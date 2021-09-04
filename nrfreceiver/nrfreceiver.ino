#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

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
    parcel getParcel;
    uint16_t res = network.read(header, &getParcel, sizeof(getParcel));
    if (res <= 0) {
      Serial.print("Failed");
    }
    Serial.print("A");
    Serial.println(getParcel.s1);
    Serial.print("B");
    Serial.println(getParcel.s2);
    Serial.print("C");
    Serial.println(getParcel.s3);
    Serial.print("D");
    Serial.println(getParcel.s4);
    Serial.print("E");
    Serial.println(getParcel.s5, 6);
    Serial.print("F");
    Serial.println(getParcel.s6, 6);
    Serial.print("G");
    Serial.println(getParcel.s7);
    Serial.print("H");
    Serial.println(getParcel.s8);
    Serial.print("I");
    Serial.println(getParcel.s9);
  }
}
