#include "Place.h"

void Place::begin(uint8_t address, Shiftduino * outputs, uint8_t greenPos, uint8_t redPos){

  _sensor.begin(address);
  _sensor.setRange(10);
  _outputs = outputs;
  _greenPos = greenPos;
  _redPos = redPos;
}

void Place::update() {

  int distance = _sensor.getDistanceCm();
  Serial.print("Distance: ");
  Serial.println(distance, DEC);
  _state = (distance <= MARGIN);
  _outputs->setPin(_greenPos, _state);
  _outputs->setPin(_redPos, !_state);
}

boolean Place::getState() {

  return _state;
}



