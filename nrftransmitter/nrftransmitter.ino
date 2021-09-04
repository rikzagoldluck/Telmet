#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Ultrasonic.h>

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
};

Ultrasonic u1(22, 26);  // An ultrasonic sensor HC-04
Ultrasonic u2(24, 28);  // An ultrasonic sensor HC-04
Ultrasonic u3(25, 29);  // An ultrasonic sensor HC-04
Ultrasonic u4(23, 27);  // An ultrasonic sensor HC-04

MPU6050 mpu;

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

//double Long, Lat;

const uint8_t APIPin = 6; // pin digital flame sensor

const uint16_t this_node = 01;       // Address of our node in Octal format
const uint16_t other_node = 00;      // Address of the other node in Octal format
const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?

TinyGPSPlus gps;

void setup(void) {
  Serial.begin(115200);
  Serial1.begin(9600);

  pinMode(APIPin, INPUT);

  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("tidak ada sensor MPU6050 yang terpasang!");
    delay(500);
  }

  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_tx/"));

  SPI.begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop() {
  network.update(); // Check the network regularly
  uint8_t api = 0;

  if (digitalRead(APIPin) == HIGH) {
    api = 1;
  }
  // Read normalized values
  Vector normAccel = mpu.readNormalizeAccel();
  // Calculate Pitch, Roll and
  int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis * normAccel.YAxis + normAccel.ZAxis * normAccel.ZAxis)) * 180.0) / M_PI;
  int roll = (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;

  sendData(u1.read(CM), u2.read(CM), u3.read(CM), u4.read(CM), api, pitch, roll);
}

void sendData(int u1, int u2, int u3, int u4, int api, int pitch, int roll) {
  parcel myParcel;
  myParcel.s1 = u1;
  myParcel.s2 = u2;
  myParcel.s3 = u3;
  myParcel.s4 = u4;
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

  unsigned long now = millis();
  if (now - last_sent >= interval) {

    // If it's time to send a message, send it!
    Serial.println("Sending...");
    //    payload_t payload = { millis(), packets_sent++ };

    Serial.println(myParcel.s1);
    Serial.println(myParcel.s2);
    Serial.println(myParcel.s3);
    Serial.println(myParcel.s4);
    Serial.println(myParcel.s5);
    Serial.println(myParcel.s6);
    Serial.println(myParcel.s7);
    Serial.println(myParcel.s8);
    Serial.println(myParcel.s9);

    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &myParcel, sizeof(myParcel));
    if (ok) Serial.println("ok.");
    else Serial.println("failed.");
    last_sent = now;
  }
}
