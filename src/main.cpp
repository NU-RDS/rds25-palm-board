#include <FlexCAN_T4.h>
#include "encoder.hpp"
#include <SPI.h>
#include <Arduino.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
#define ENCODER1_CS 4 // CS Pin 4. This can be replaced with other CS Pins.
Encoder encoder1(ENCODER1_CS, 0);  // All SPI ports on the palm board are SPI_0

void canSniff(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}

void setup(void) {
  Serial.begin(115200); delay(400);
  pinMode(6, OUTPUT); digitalWrite(6, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(1000000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();

  // Initialize SPI bus
  SPI.begin();
  pinMode(ENCODER1_CS, OUTPUT);
  digitalWrite(ENCODER1_CS, HIGH);
}

void loop() {
  Can0.events();
  static uint32_t timeout = millis();
  if ( millis() - timeout > 200 ) {
    // Send random CAN messages
    CAN_message_t msg;
    msg.id = random(0x1,0x7FE);
    for ( uint8_t i = 0; i < 8; i++ ) msg.buf[i] = i + 1;
    Can0.write(msg);
    timeout = millis();

    // Read encoders
    float angle1 = encoder1.readEncoderDeg();
    Serial.print("Encoder 1 (Pin ");
    Serial.print(ENCODER1_CS);
    Serial.print("): ");
    Serial.print(angle1);
    Serial.println(" degrees");
  }

}
