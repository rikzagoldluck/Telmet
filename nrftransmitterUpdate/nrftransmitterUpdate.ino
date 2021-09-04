#include <SPI.h>
#include <printf.h>
#include <RF24.h>
#include <Ultrasonic.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <MPU6050.h>

#define COUNT 10

uint8_t _success = 0;
uint8_t _fail = 0;
unsigned long _startTime = 0;

struct parcel {
  uint16_t s1;
  uint16_t s2;
  uint16_t s3;
  uint16_t s4;
  double s5;
  double s6;
  uint8_t s7;
  int s8;
  int s9;
  unsigned long t;
  unsigned long tavg;
  uint8_t _success;
  uint8_t _fail;
  uint8_t _ratio;
};

struct res {
  char ok[2];
};

res getRes;

Ultrasonic u1(22, 26);  // An ultrasonic sensor HC-04
Ultrasonic u2(24, 28);  // An ultrasonic sensor HC-04
Ultrasonic u3(25, 29);  // An ultrasonic sensor HC-04
Ultrasonic u4(23, 27);  // An ultrasonic sensor HC-04

unsigned long transmit = 0;
bool radioNumber = 1;
bool role = false;

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin
MPU6050 mpu;

uint8_t address[][6] = {"1Node", "2Node"};
const uint8_t APIPin = 6; // pin digital flame sensor
TinyGPSPlus gps;

void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(APIPin, INPUT);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }


  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("tidak ada sensor MPU6050 yang terpasang!");
    delay(500);
  }

  //  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads() ;
  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]);     // always uses pipe 1
  radio.openReadingPipe(1, address[0]); // using pipe 0

  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening(); // put radio in RX mode
  }

  // For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  //  radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup

void loop() {

  if (role) {
    unsigned long loop_start = millis();
    // This device is a TX node
    uint8_t api = 0;

    if (digitalRead(APIPin) == HIGH) {
      api = 1;
    }
    // Read normalized values
    Vector normAccel = mpu.readNormalizeAccel();
    // Calculate Pitch, Roll and
    int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis * normAccel.YAxis + normAccel.ZAxis * normAccel.ZAxis)) * 180.0) / M_PI;
    int roll = (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;

    parcel myParcel;
    myParcel.s1 = u1.read(CM);
    myParcel.s2 = u2.read(CM);
    myParcel.s3 = u3.read(CM);
    myParcel.s4 = u4.read(CM);
    while (Serial1.available() > 0) {
      if (gps.encode(Serial1.read())) {
        if (gps.location.isValid())
        {
          myParcel.s5 = gps.location.lng();
          myParcel.s6 = gps.location.lat();
        }
      }
    }
    myParcel.s7 = api;
    myParcel.s8 = pitch;
    myParcel.s9 = roll;
    myParcel.t = transmit;

    unsigned long start_timer = micros();                   // start the timer
    bool report = radio.write(&myParcel, sizeof(myParcel));      // transmit & save the report
    unsigned long end_timer = micros();
    radio.startListening();
    // Serial.println(F("delivery success."));
    // printf("Time: %i ", millis() - loop_start);

    while ( !radio.available() && (millis() - loop_start) < 200) {
      // Serial.println(F("waiting."));
    }
    if (millis() - loop_start >= 200) {
      // printf("Failed. Timeout: %i...", millis() - loop_start);
      _fail++;
    } else {
      // get the telemetry data
      radio.read( &getRes, sizeof(getRes));
      // Serial.print("Got response ");
      _success++;
    }

    if (_fail + _success >= COUNT)
    {
      // Persentase kegagalan      
      uint8_t _ratio = 100 * _fail / (_fail + _success);

      // waktu kirim 10 data
      Serial.print("Time(ms) ");
      _startTime = (millis() - _startTime);
      Serial.print(_startTime);
      myParcel.tavg = _startTime;

      // jumlah paket terkirim
      Serial.print(" success ");
      Serial.print(_success);
      myParcel._success = _success;

      // jumlah paket gagal kirim
      Serial.print(" timeout ");
      Serial.print(_fail);
      myParcel._fail = _fail;

      
      Serial.print(" Failed ");
      Serial.print(_ratio);
      myParcel._ratio = _ratio;
      Serial.print("% ");
      for (int _i = 0; _i < _ratio; _i++) Serial.print("*");
      Serial.print("res: ");
      Serial.println(getRes.ok);
      _success = 0;
      _fail = 0;
      _startTime = millis();
    }
    // end the timer

    if (report) {
      transmit = end_timer - start_timer;
      Serial.print(F("Transmission successful! "));          // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(transmit);                 // print the timer result
      Serial.println(F(" us. "));
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }
    //  delay(1000);  // slow transmissions down by 1 second
  } else {
    // This device is a RX node
    parcel myParcel;
    uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getDynamicPayloadSize(); // get the size of the payload
      radio.read(&myParcel, bytes);            // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);                    // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);                     // print the pipe number
      Serial.print(F(": "));
      //      Serial.println(payload);                // print the payload's value
    }
  } // role
  if (!role) {
    role = true;
    radio.stopListening();
  } else if (role) {
    role = false;
    radio.startListening();
  }
  //  }
} // loop
