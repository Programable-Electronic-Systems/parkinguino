#ifndef PLACE_H
#define PLACE_H

#include "Arduino.h"
#include <Wire.h>
#include "Shiftduino.h"
#include "SRF08.h"

#define MARGIN    16

class Place {
  
  public:
    void begin(uint8_t address, Shiftduino * outputs, uint8_t greenPos, uint8_t redPos);
    void update();
    boolean getState();
  
  private:
    SRF08 _sensor;
    Shiftduino * _outputs;
    uint8_t _greenPos;
    uint8_t _redPos;
    boolean _state;
};

#endif
