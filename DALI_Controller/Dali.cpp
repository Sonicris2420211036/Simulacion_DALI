#include "Arduino.h"
#include "Driver.h"
#include "Dali.h"

#define TRANSMISSION_RATE_US 440
#define DELAY_BETWEEN_FRAMES 9200


// ? /////////////////////////////
// ? Contructor
// ? /////////////////////////////
Dali::Dali(byte txPin)
{
  Driver driver(txPin, TRANSMISSION_RATE_US);
  this->driver = driver;
  this->driver.setupPin();
};

// ? /////////////////////////////
// ? Helpers
// ? /////////////////////////////
void Dali::prepareShortAddress(byte address)
{
  this->address = address;
  this->Y = false;
};

void Dali::prepareGroupAddress(byte address)
{
  this->address = address & 0b001111;
  this->Y = true;
};

void Dali::prepareBroadcast()
{
  this->address = 0b111111;
  this->Y = true;
};

// ? /////////////////////////////
// ? Commands for lamp power
// ? /////////////////////////////
void Dali::dim(byte dimLevel)
{
  this->driver.sendForwardFrame(this->Y, this->address, 0, dimLevel);
};

void Dali::dimRGB(byte redLevel, byte greenLevel, byte blueLevel){
  this->driver.sendForwardFrame(this->Y, this->address, 0, redLevel);
  // Delay between frames (could need to be adjusted)
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 0, greenLevel);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 0, blueLevel);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
}

void Dali::switchOff()
{
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x00);
};

void Dali::turnBrighterWithStepSpeed() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x01);
};

void Dali::turnDarkerWithStepSpeed() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x02);
};

void Dali::increasePowerValueByStepBounded() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x03);
};

void Dali::decreasePowerValueByStepBounded() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x04);
};

void Dali::setPowerToMaximum() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x05);
};

void Dali::setPowerToMinimum() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x06);
};

void Dali::decreasePowerValueByStepUnbounded() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x07);
};

void Dali::increasePowerValueByStepUnbounded() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x08);
};

void Dali::setStoredPowerScene(byte sceneCommand) {
  if (sceneCommand < 16 || sceneCommand > 31) {
    Serial.println("Invalid scene number (n) out of range (16-31)");
    return;
  };

  this->driver.sendForwardFrame(this->Y, this->address, 1, sceneCommand);
};

void Dali::resetParameters() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x20);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x20);
};

void Dali::saveBrightLevelToDTR() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x21);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x21);
};

void Dali::saveDTRAsMaximumPowerValue() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2A);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2A);
};

void Dali::saveDTRAsMinimumPowerValue() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2B);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2B);
};

void Dali::saveDTRAsFaultConditionPowerValue() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2C);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2C);
};

void Dali::saveDTRAsSwitchOnPowerValue() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2D);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2D);
};

void Dali::saveDTRAsStepTime() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2E);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2E);
};

void Dali::saveDTRAsStepSpeed() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2F);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x2F);
};

void Dali::saveDTRAsSceneBrightness(byte sceneCommand) {
  if (sceneCommand < 64 || sceneCommand > 79) {
    Serial.println("Invalid scene number (n) out of range (64-79)");
    return;
  };

  this->driver.sendForwardFrame(this->Y, this->address, 1, sceneCommand);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, sceneCommand);
};

void Dali::deleteSelectedScene(byte sceneCommand) {
  if (sceneCommand < 80 || sceneCommand > 95) {
    Serial.println("Invalid scene number (n) out of range (80-95)");
    return;
  };

  this->driver.sendForwardFrame(this->Y, this->address, 1, sceneCommand);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, sceneCommand);
};

void Dali::addToGroup(byte groupCommand) {
  if (groupCommand < 96 || groupCommand > 111) {
    Serial.println("Invalid group number (n) out of range (96-111)");
    return;
  };

  this->driver.sendForwardFrame(this->Y, this->address, 1, groupCommand);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, groupCommand);
};

void Dali::deleteOfGroup(byte groupCommand) {
  if (groupCommand < 112 || groupCommand > 127) {
    Serial.println("Invalid group number (n) out of range (112-127)");
    return;
  };

  this->driver.sendForwardFrame(this->Y, this->address, 1, groupCommand);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, groupCommand);
};

void Dali::saveDTRValueAsShortAddress() {
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x80);
  delayMicroseconds(DELAY_BETWEEN_FRAMES);
  this->driver.sendForwardFrame(this->Y, this->address, 1, 0x80);
};

