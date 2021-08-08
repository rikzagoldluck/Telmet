#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <Telmet.h>

RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);     // Network uses that radio
const uint16_t this_node = 00;  // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 01; // Address of the other node in Octal format


void setup(void) {
  Serial.begin(115200);
  if (!Serial) {
    // some boards need this because of native USB capability
    Serial.println("Failed");
  }
  SPI.begin();
  radio.begin();

  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void) {

  network.update();                  // Check the network regularly

    while (network.available()) {      // Is there anything ready for us?

    RF24NetworkHeader header;        // If so, grab it and print it out
    tampung sensors;
    uint16_t res = network.read(header, &sensors, sizeof(sensors));
    if (res <= 0) {
      Serial.print("Failed");
    }
    Serial.print("A");
    Serial.println(sensors.s1);
    Serial.print("B");
    Serial.println(sensors.s2);
    Serial.print("C");
    Serial.println(sensors.s3);
    Serial.print("D");
    Serial.println(sensors.s4);
    Serial.print("E");
    Serial.println(sensors.s5);
    Serial.print("F");
    Serial.println(sensors.s6);
    Serial.print("G");
    Serial.println(sensors.s7);
    Serial.print("H");
    Serial.println(sensors.s8);
    Serial.print("I");
    Serial.println(sensors.s9);
    Serial.print("J");
    Serial.println(sensors.s10);
  }
}
