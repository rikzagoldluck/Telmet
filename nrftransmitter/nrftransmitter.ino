#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <Telmet.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);          // Network uses that radio

static const uint8_t RXPin = 4, TXPin = 3;
double Long, Lat = 0;
const uint8_t APIPin = 5; // pin digital flame sensor
const uint16_t this_node = 01;       // Address of our node in Octal format
const uint16_t other_node = 00;      // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit

unsigned long last_sent;             // When did we last send?

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup(void) {
  Serial.begin(115200);
  ss.begin(9600);
  pinMode(APIPin, INPUT);
  // u1 = 22, 26 ; u2 = 23 , 27 ; u3 = 24 , 28; u4 = 25, 29
  for ( uint8_t i = 22 ; i <= 25 ; i++ ) {
    setUltra(i , i + 4 );
  }
  
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("tidak ada sensor MPU6050 yang terpasang!");
    delay(500);
  }
  //Kalibrasi gyrometer
  mpu.calibrateGyro();
  mpu.setThreshold(3);
  
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
  
  uint8_t api = 0;
  network.update(); // Check the network regularly\

  if (digitalRead(APIPin) == HIGH) {
    api = 1;
  }

  unsigned long now = millis();
  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;

    Serial.print("Sending...");
    //    payload_t payload = { millis(), packets_sent++ };
    tampung paket;

    paket.s1 = getUltra(22, 26);
    paket.s2 = getUltra(23, 27);
    paket.s3 = getUltra(24, 28);
    paket.s4 = getUltra(25, 29);
    while (ss.available() > 0) {
      if (gps.encode(ss.read())) getGPS();
      paket.s5 = (float)Long;
      paket.s6 = (float)Lat;
    }
    paket.s7 = api;
    Vector rawGyro = mpu.readRawGyro();
    paket.s8 = rawGyro.XAxis;
    paket.s9 = rawGyro.YAxis;
    paket.s10 = rawGyro.ZAxis;

    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &paket, sizeof(paket));
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
  }
}

void getGPS() {
  if (gps.location.isUpdated() || gps.location.isValid()) {
    Long = gps.location.lng();
    Lat = gps.location.lat();
  }
}
