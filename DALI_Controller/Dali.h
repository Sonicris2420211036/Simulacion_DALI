#ifndef DALI_H
#define DALI_H
#include <Arduino.h>
#include "Driver.h"

class Dali
{
private:
  byte address;
  bool Y;
  Driver driver;

public:
  // Constructor
  Dali(byte txPin);

  // Helpers
  void prepareShortAddress(byte address);
  void prepareGroupAddress(byte address);
  void prepareBroadcast();

  // Dim level
  void dim(byte dimLevel);
  void dimRGB(byte redLevel, byte greenLevel, byte blueLevel);

  // Commands for lamp power
  // https://infosys.beckhoff.com/english.php?content=../content/1033/kl6811/2214047371.html&id=
  void switchOff();
  void turnBrighterWithStepSpeed();
  void turnDarkerWithStepSpeed();
  void increasePowerValueByStepBounded();
  void decreasePowerValueByStepBounded();
  void setPowerToMaximum();
  void setPowerToMinimum();
  void increasePowerValueByStepUnbounded();
  void decreasePowerValueByStepUnbounded();
  void setStoredPowerScene(byte sceneCommand);
  void resetParameters();
  void saveBrightLevelToDTR();
  void saveDTRAsMaximumPowerValue();
  void saveDTRAsMinimumPowerValue();
  void saveDTRAsFaultConditionPowerValue();
  void saveDTRAsSwitchOnPowerValue();
  void saveDTRAsStepTime();
  void saveDTRAsStepSpeed();
  void saveDTRAsSceneBrightness(byte sceneCommand);
  void deleteSelectedScene(byte sceneCommand);
  void addToGroup(byte groupCommand);
  void deleteOfGroup(byte groupCommand);
  void saveDTRValueAsShortAddress();
};

#endif