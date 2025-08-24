#include <ModeFree.h>
#include <math.h>
#include <Arduino.h>
#include <../../include/tracks.h>

ModeFree::ModeFree(SystemIO* systemIo) : Mode(systemIo) {

}

#define TRACK_LAUNCH_ENGINES_START_1    11000
#define TRACK_LAUNCH_ENGINES_START_2    12000
#define TRACK_LAUNCH_ENGINES_START_3    13000
#define TRACK_LAUNCH_ENGINES_START_4    14000
#define TRACK_LAUNCH_ENGINES_START_5    15000
#define TRACK_LAUNCH_ENGINE_FAILURE     46000
#define TRACK_LAUNCH_END                60000
#define TRACK_LAND_FUEL_LIGHT           98000
#define TRACK_LAND_CONTACT_LIGHT        108000
#define TRACK_LAND_END                  143000
#define TRACK_LAND_FUEL_QUANT           9000
#define TRACK_LAND_ALT_QUANT            5000

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
    case TRACK_LAUNCH:
        uint8_t e[5] = {
            elapsed > TRACK_LAUNCH_ENGINES_START_1 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_2 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_3 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINES_START_4 ? 1 : 0,
            elapsed > TRACK_LAUNCH_ENGINE_FAILURE ? 2 : (elapsed > TRACK_LAUNCH_ENGINES_START_5 ? 1 : 0)
        };
        this->systemIo->setEngineLights(e);
        this->systemIo->setMasterAlarm(elapsed > TRACK_LAUNCH_ENGINE_FAILURE);

        double pcnt = (double)elapsed / (double)TRACK_LAUNCH_END;
        int fuel = TRACK_LAND_FUEL_QUANT - (int)(pcnt * TRACK_LAND_FUEL_QUANT);
        this->systemIo->setFuel(fuel);
        int fuel = (int)(pcnt * TRACK_LAND_ALT_QUANT);
        this->systemIo->setAltitude(fuel);

        if (elapsed > TRACK_LAUNCH_ENGINE_FAILURE && this->systemIo->getMasterAlarm()) {
            this->reset();
        }
        break;
    case TRACK_LAND:
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
        
        double pcnt = (double)elapsed / (double)TRACK_LAND_END;
        int fuel = TRACK_LAND_FUEL_QUANT - (int)(pcnt * TRACK_LAND_FUEL_QUANT);
        this->systemIo->setFuel(fuel);
        int fuel = TRACK_LAND_ALT_QUANT - (int)(pcnt * TRACK_LAND_ALT_QUANT);
        this->systemIo->setAltitude(fuel);

        if (elapsed > TRACK_LAND_END) {
            this->reset();
        }
        break;
    case TRACK_APOLLO13:
        this->systemIo->setMasterAlarm(true);
        if (this->systemIo->getMasterAlarm()) {
            this->reset();
        }
        break;
    case TRACK_MAIN_CHUTE: {
        double pcnt = (double)elapsed / (double)TRACK_LAND_END;
        int fuel = TRACK_LAND_ALT_QUANT - (int)(pcnt * TRACK_LAND_ALT_QUANT);
        this->systemIo->setAltitude(fuel);
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
