#include <SPI.h>
#include <printf.h>
#include <RF24.h>

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

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

parcel myParcel;

struct res {
  char ok[2];
};

res setRes;
// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};
//bool radioNumber = 0; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
//bool role = false;

void setup() {

  Serial.begin(2000000);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }
  radio.setDataRate(RF24_2MBPS);
  //  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radio.enableDynamicPayloads() ;
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]); // using pipe 1

  radio.startListening(); // put radio in RX mode
  //  }

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup

void loop() {
  uint8_t pipe;
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getDynamicPayloadSize(); // get the size of the payload
    radio.read(&myParcel, bytes);            // fetch payload from FIFO
    //    Serial.print(F("Received "));
    //    Serial.println(bytes);
    memcpy(setRes.ok , "ok", 2);
    printAll();
    radio.stopListening();
    radio.write( &setRes, sizeof(setRes) );
    // Serial.print("Sent response ");
    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
} // loop

void printAll() {
  Serial.print("0");
  Serial.println(myParcel.s1);
  Serial.print("1");// print the payload's value
  Serial.println(myParcel.s2);
  Serial.print("2");
  Serial.println(myParcel.s3);
  Serial.print("3");
  Serial.println(myParcel.s4);
  Serial.print("4");
  Serial.println(myParcel.s5);
  Serial.print("5");
  Serial.println(myParcel.s6);
  Serial.print("6");
  Serial.println(myParcel.s7);
  Serial.print("7");
  Serial.println(myParcel.s8);
  Serial.print("8");
  Serial.println(myParcel.s9);
  Serial.print("9");
  Serial.println(myParcel.t);
  Serial.print("10");
  Serial.println(myParcel.tavg);
  Serial.print("11");
  Serial.println(myParcel._success);
  Serial.print("12");
  Serial.println(myParcel._fail);
  Serial.print("13");
  Serial.println(myParcel._ratio);
  delay(100);
}
