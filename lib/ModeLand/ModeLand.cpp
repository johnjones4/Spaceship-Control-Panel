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
    this->start = millis();
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

            double thrustPcnt = this->systemIo->getThrottle();
            this->fuel = this->fuel + ((thrustPcnt * FUEL_USAGE_RATE) * deltaTime);
            thrustAccel = MAX_THRUST * thrustPcnt;
            // thrustAccel = MAX_THRUST * pow(1 - (this->fuel / FUEL_INITIAL), 2)) * thrustPcnt * -1;
        }
        double netAccel = thrustAccel + GRAVITY;
        this->deltaZ = this->deltaZ + netAccel * deltaTime;
        this->altitude = this->altitude + (this->deltaZ * deltaTime);
        this->lastStep = now;
    }

    bool isDown = this->altitude <= ALTITUDE_MIN;
    bool isOverun = isDown && (abs(this->deltaZ) > MIN_ABS_DELTA || abs(this->deltaX) > MIN_ABS_DELTA || abs(this->deltaY) > MIN_ABS_DELTA);

    this->systemIo->setAltitude((int)this->altitude);
    this->systemIo->setFuel(this->fuel > 0 ? (int)this->fuel : 0);
    this->systemIo->setFuelLight(this->fuel < FUEL_MIN);
    this->systemIo->setContactLight(this->altitude <= ALTITUDE_LIGHT_MIN);
    this->systemIo->setMasterAlarm(isOverun);

    double width = (double)(this->systemIo->getTFT()->width()) / 2.0;
    double height = (double)(this->systemIo->getTFT()->height()) / 2.0;
    double xLine = (int16_t)(width - (max(-1, min(1, this->deltaY / MAX_VELOCITY_X)) * width));
    double yLine = (int16_t)(height - (max(-1, min(1, this->deltaX / MAX_VELOCITY_Y)) * height));
    
    bool drawLine = true;
    if (this->lastXLine > 0 && this->lastYLine > 0) {        
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
    }

    if (start > 0 && isDown) {
        start = 0;
        if (!isOverun) {
            this->systemIo->playTrack(TRACK_EAGLE_LANDED);
        }
    }
    
    if (this->systemIo->getMasterAlarm()) {
        this->reset();
    }
}