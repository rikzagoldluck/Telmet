/*
 * 
 * Semoga bisa mempermudah
 * 
 */
#include <Telmet.h>
void setup() 
{
  // Setup Serial communication to print result on Serial Monitor
  Serial.begin(9600);
  // Setup 4 sensor ultrasonik untuk arduino mega
  // u1 = 22, 26 ; u2 = 23 , 27 ; u3 = 24 , 28; u4 = 25, 29
  
  for ( uint16_t i = 22 ; i <= 25 ; i++ ) {
    setUltra(i , i + 4 );
  }
}
void loop() {
  tampung payload;

  payload.s1 = getUltra(22, 26);
  payload.s2 = getUltra(23, 27);
  payload.s3 = getUltra(24, 28);
  payload.s4 = getUltra(25, 29);
}