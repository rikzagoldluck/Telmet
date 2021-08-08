#ifndef Telmet_h
#define Telmet_h
#include <Arduino.h>

struct tampung {                   
  uint16_t s1;
  uint16_t s2;
  uint16_t s3;
  uint16_t s4;
  float s5;
  float s6;
  uint8_t s7;
  integer s8;
  integer s9;
  integer s10;
};

void setUltra( uint8_t trig, uint8_t echo );
uint16_t getUltra( uint8_t trig, uint8_t echo );
#endif