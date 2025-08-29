#include <ModeLand.h>
#include <math.h>
#include <Arduino.h>
#include <../../include/tracks.h>

ModeLand::ModeLand(SystemIO* systemIo) : Mode(systemIo) {

}

void ModeLand::reset() {
    this->deltaX = DELTA_X_INITIAL;
    this->deltaY = DELTA_Y_INITIAL;
    this->deltaZ = GRAVITY;
    this->thrust = THRUST_INITIAL;
    this->altitude = ALTITUDE_INITIAL;
    this->fuel = FUEL_INITIAL;
    this->playedFuel = false;
    this->start = 0;
    this->systemIo->stopTrack();
}

void ModeLand::step() {
    if (start > 0) {
        unsigned long now = millis();
        double deltaTime = ((double)(now - this->lastStep) / 1000);
        double thrustAccel = 0;
        if (this->fuel > 0) {
            t_direction nextDirection = this->systemIo->getDirection();
            switch (nextDirection)
            {
            case Neutral:
                break;
            case North:
                this->deltaY += DIRECTION_DELTA_DELTA;
                break;
            case South:
                this->deltaY -= DIRECTION_DELTA_DELTA;
                break;
            case East:
                this->deltaX += DIRECTION_DELTA_DELTA;
                break;
            case West:
                this->deltaX -= DIRECTION_DELTA_DELTA;
                break;
            }

            double noise = (((double)random(1, 100) / 100.0) * 2.0) - 1;
            this->deltaX += noise * RANDOM_VELOCITY_CHANGE_MAX;
            noise = (((double)random(1, 100) / 100.0) * 2.0) - 1;
            this->deltaY += noise * RANDOM_VELOCITY_CHANGE_MAX;

            double thrustPcnt = this->systemIo->getThrottle();
            this->fuel = this->fuel + ((thrustPcnt * FUEL_USAGE_RATE) * deltaTime);
            thrustAccel = MAX_THRUST * thrustPcnt;
        }
        double netAccel = thrustAccel + GRAVITY;
        this->deltaZ = this->deltaZ + netAccel * deltaTime;
        this->altitude = max(0, this->altitude + (this->deltaZ * deltaTime));
        this->lastStep = now;
    }

    bool isDown = this->altitude <= ALTITUDE_MIN;
    bool isOverun = isDown && (abs(this->deltaZ) > MIN_ABS_DELTA || abs(this->deltaX) > MIN_ABS_DELTA || abs(this->deltaY) > MIN_ABS_DELTA);

    this->systemIo->setAltitude((int)this->altitude);
    this->systemIo->setFuel(this->fuel > 0 ? (int)this->fuel : 0);
    this->systemIo->setFuelLight(this->fuel < FUEL_MIN);
    this->systemIo->setContactLight(this->altitude <= ALTITUDE_LIGHT_MIN);
    this->systemIo->setMasterAlarm(isOverun);

    double xLine = cartesienPlotCoord(MAX_VELOCITY_Y, this->deltaY, this->systemIo->getTFT()->width());
    double yLine = cartesienPlotCoord(MAX_VELOCITY_X, this->deltaX, this->systemIo->getTFT()->height());
    
    bool drawLine = true;
    if (this->lastXLine >= 0 && this->lastYLine >= 0) {        
        if (this->lastXLine != xLine || this->lastYLine != yLine) {
            this->systemIo->getTFT()->drawLine((int16_t)lastXLine, 0, (int16_t)lastXLine, this->systemIo->getTFT()->height(), ST77XX_WHITE);
            this->systemIo->getTFT()->drawLine(0, (int16_t)lastYLine, this->systemIo->getTFT()->width(), (int16_t)lastYLine, ST77XX_WHITE);    
        } else {
            drawLine = false;
        }
    }
    if (drawLine) {
        this->lastXLine = xLine;
        this->lastYLine = yLine;
        this->systemIo->getTFT()->drawLine((int16_t)xLine, 0, (int16_t)xLine, this->systemIo->getTFT()->height(), ST77XX_BLACK);
        this->systemIo->getTFT()->drawLine(0, (int16_t)yLine, this->systemIo->getTFT()->width(), (int16_t)yLine, ST77XX_BLACK);
        double x = cartesienPlotCoord(MAX_VELOCITY_Y, 0, this->systemIo->getTFT()->width());
        double y = cartesienPlotCoord(MAX_VELOCITY_X, 0, this->systemIo->getTFT()->height());
        double r = (MIN_ABS_DELTA / MAX_VELOCITY_X) * (double)(this->systemIo->getTFT()->height() / 2);
        this->systemIo->getTFT()->drawCircle(x, y, r, ST77XX_BLACK);
    }

    if (this->fuel < FUEL_MIN && !this->playedFuel) {
        this->systemIo->playTrack(TRACK_60_SECS_FUEL);
        this->playedFuel = true;
    }

    if (start > 0 && isDown) {
        start = 0;
        if (!isOverun) {
            this->systemIo->playTrack(TRACK_EAGLE_LANDED);
        }
    }
    
    if (this->systemIo->getMasterAlarm() || this->systemIo->getReset()) {
        this->reset();
    }

    if (start == 0 && this->systemIo->getEngineArm() && this->systemIo->getLand()) {
        this->systemIo->playTrack(TRACK_GO_FOR_LANDING);
        lastStep = millis();
        start = millis();        
    }
}

int16_t ModeLand::cartesienPlotCoord(double maxVal, double value, int16_t length) {
    double l1 = (double)length / 2;
    double constrainedValue = max(maxVal * -1, min(maxVal, value));
    return (int16_t)(l1 - ((constrainedValue / maxVal) * l1));
}