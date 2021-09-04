#include <Telmet.h>
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t node1 = 01;
const uint16_t SApi = 50;
const unsigned long interval = 3000; // How often (in ms) to send 'hello world' to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  pinMode(SApi, INPUT);
  for ( uint16_t i = 24 ; i <= 27 ; i++ ) {
    setUltra(i , i + 4 );
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  tampung sensors;
  network.update();

  sensors.s1 = getUltra(24, 28);
  sensors.s2 = getUltra(25, 29);
  sensors.s3 = getUltra(26, 30);
  sensors.s4 = getUltra(27, 31);
  sensors.s5 = digitalRead(SApi);
  sensors.s6 = random(1, 10);
  sensors.s7 = random(1, 10);
  sensors.s8 = random(1, 10);
  sensors.s9 = random(1, 10);

//  unsigned long now = millis();
//
//  // If it's time to send a message, send it!
//  if (now - last_sent >= interval) {
//    last_sent = now;

    Serial.print("Sending...");
    RF24NetworkHeader header(/*to node*/ node1);
    bool ok = network.write(header, &sensors, sizeof(sensors));
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
//  }
}
