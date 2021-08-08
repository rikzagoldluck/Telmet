#include "Telmet.h"

void setUltra( uint8_t trig, uint8_t echo ) {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}


uint16_t getUltra( uint8_t trig, uint8_t echo ) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  const unsigned long duration = pulseIn(echo, HIGH);

  uint8_t distance = duration / 29 / 2;
  if ( distance > 400 ){
    distance = 400;
  }
  return distance;
}