#include <ModeFree.h>
#include <math.h>
#include <Arduino.h>
#include <../../include/tracks.h>

ModeFree::ModeFree(SystemIO* systemIo) : Mode(systemIo) {

}

#define MAX_FUEL    9999
#define MAX_ALT     9999

#define TRACK_LAUNCH_ENGINES_START_1    11000
#define TRACK_LAUNCH_ENGINES_START_2    13000
#define TRACK_LAUNCH_ENGINES_START_3    15000
#define TRACK_LAUNCH_ENGINES_START_4    17000
#define TRACK_LAUNCH_ENGINES_START_5    19000
#define TRACK_LAUNCH_LAUNCH             22000
#define TRACK_LAUNCH_END                60000
#define TRACK_LAND_FUEL_LIGHT           98000
#define TRACK_LAND_CONTACT_LIGHT        108000
#define TRACK_LAND_END                  143000
#define TRACK_ENGINE_ARM_IGNITION       9000
#define TRACK_ENGINE_ARM_END            38000
#define TRACK_TOWER_JET_ENGINE_FAIL     11000
#define TRACK_TOWER_JET_END             63000

void ModeFree::reset() {
    this->lastAudioStart = 0;
    this->trackPlaying = NO_TRACK;
    uint8_t e[5] = {0,0,0,0,0};
    this->systemIo->setEngineLights(e);
    this->systemIo->setMasterAlarm(false);
    this->systemIo->setContactLight(false);
    this->systemIo->setFuelLight(false);
    this->systemIo->setFuel(0);
    this->systemIo->setAltitude(0);
    this->systemIo->stopTrack();
}

void ModeFree::step() {
    if (this->systemIo->getReset()) {
        this->reset();
    } else if (this->trackPlaying != NO_TRACK) {
        this->handlePlayingTrack();
    } else {
        this->handleNextPlayingTrack();
    }
}

void ModeFree::handlePlayingTrack() {
    unsigned long elapsed = millis() - this->lastAudioStart;
    switch (this->trackPlaying)
    {
    case TRACK_LAUNCH: {
        uint8_t e[5] = {
            elapsed > TRACK_LAUNCH_ENGINES_START_1 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_2 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_3 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_4 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_5 ? 1 : 0
        };
        this->systemIo->setEngineLights(e);

        double fpcnt = min(1, max(0, ((double)elapsed - TRACK_LAUNCH_ENGINES_START_1)) / (double)(TRACK_LAUNCH_END - TRACK_LAUNCH_ENGINES_START_1));
        int fuel = MAX_FUEL - (int)(fpcnt * MAX_FUEL);
        this->systemIo->setFuel(fuel);
        double apcnt = min(1, max(0, ((double)elapsed - TRACK_LAUNCH_LAUNCH)) / (double)(TRACK_LAUNCH_END - TRACK_LAUNCH_LAUNCH));
        int alt = (int)(apcnt * MAX_ALT);
        this->systemIo->setAltitude(alt);

        if (elapsed > TRACK_LAUNCH_END) {
            this->reset();
        }
        break;
    }
    case TRACK_LAND: {
        uint8_t e[5] = {
            0,
            0,
            0,
            0,
            elapsed < TRACK_LAND_CONTACT_LIGHT
        };
        this->systemIo->setEngineLights(e);
        this->systemIo->setFuelLight(TRACK_LAND_FUEL_LIGHT);
        this->systemIo->setContactLight(elapsed > TRACK_LAND_CONTACT_LIGHT);
        
        double pcnt = min(1, (double)elapsed / (double)TRACK_LAND_CONTACT_LIGHT);
        int fuel = MAX_FUEL - (int)(pcnt * MAX_FUEL);
        this->systemIo->setFuel(fuel);
        int alt = MAX_ALT - (int)(pcnt * MAX_ALT);
        this->systemIo->setAltitude(alt);

        if (elapsed > TRACK_LAND_END) {
            this->reset();
        }
        break;
    }
    case TRACK_APOLLO13:
        this->systemIo->setMasterAlarm(true);
        if (this->systemIo->getMasterAlarm()) {
            this->reset();
        }
        break;
    case TRACK_MAIN_CHUTE: {
        double pcnt = (double)elapsed / (double)TRACK_LAND_END;
        int alt = MAX_ALT - (int)(pcnt * MAX_ALT);
        this->systemIo->setAltitude(alt);
        break;
    }
    case TRACK_ENGINE_ARM: {
        uint8_t e[5] = {
            0,
            0,
            0,
            0,
            elapsed > TRACK_ENGINE_ARM_IGNITION ? 1 : 0,
        };
        this->systemIo->setEngineLights(e);
        if (elapsed > TRACK_ENGINE_ARM_END) {
            this->reset();
        }
        break;
    }
    case TRACK_TOWER_JET: {
        uint8_t e[5] = {
            1,
            1,
            1,
            1,
            elapsed > TRACK_TOWER_JET_ENGINE_FAIL ? 2 : 1
        };
        this->systemIo->setEngineLights(e);
        this->systemIo->setMasterAlarm(elapsed > TRACK_TOWER_JET_ENGINE_FAIL);
        double pcnt = min(1, (double)(elapsed) / (double)(TRACK_TOWER_JET_END));
        int fuel = MAX_FUEL - (int)(pcnt * (MAX_FUEL/2));
        this->systemIo->setFuel(fuel);
        int alt = (MAX_ALT/2) + (int)(pcnt * (MAX_ALT/2));
        this->systemIo->setAltitude(alt);
        if (this->systemIo->getMasterAlarm()) {
            this->reset();
        }
        break;
    }
    default:
        break;
    }
}

void ModeFree::handleNextPlayingTrack() {
    uint8_t next = NO_TRACK;
    if (this->systemIo->getLaunch()) {
        next = TRACK_LAUNCH;
    } else if (this->systemIo->getLand()) {
        next = TRACK_LAND;
    } else if (this->systemIo->getRetract()) {
        next = TRACK_RETRACT;
    } else if (this->systemIo->getO2Stir()) {
        next = TRACK_APOLLO13;
    } else if (this->systemIo->getEngineArm()) {
        next = TRACK_ENGINE_ARM;
    } else if (this->systemIo->getTowerJet()) {
        next = TRACK_TOWER_JET;
    } else if (this->systemIo->getMainChute()) {
        next = TRACK_MAIN_CHUTE;
    }
    if (next != NO_TRACK) {
        this->systemIo->playTrack(next);
        this->trackPlaying = next;
        this->lastAudioStart = millis();
    }
}
