#include <LiquidCrystal.h>
#include "Dali.h"
#include <string.h>

// LCD pins config
const int rs = 22, en = 23, d4 = 5, d5 = 18, d6 = 19, d7 = 21;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Control buttons
const int CANCEL_PIN = 4;
const int OK_PIN = 2;
const int ADDRESS_PIN = 15;
const int DIM_PIN = 14;
const int COMMAND_PIN = 13;

// Signal output pin
const int TX_PIN = 12;

// DALI Interface object
Dali dali(TX_PIN);

// Analog potentiometer
const int POT_PIN = 34;
long analog;
long aux, prevAux;

// Buttons state
int cancelBtn;
int okBtn;
int addressBtn;
int dimBtn;
int commandBtn;

// Address variables
const char addresses[4] = {'I', 'G', 'B', 'D'};
char addressType = 'I'; // Individual ballast: I, Group: G, B: Broadcast, D: DTR Value
char prevAddressType = 'I';
int address = 0;
int prevAddress = 0;
int addressIndex = 4, prevAddressIndex = 0;
bool addressMenu = false;
int addressMenuCount = 0;

// Dim variables
bool dimMenu = false;
bool dimRGB = false;
bool startDimChanging = false;
int bright = 0;
int red, blue, green = 0;
char currentColor = 'R';
bool sendRGB = false;

// This variable is whether the user is going to send an arc power level or a command
bool sendBright = false;

// Command variables
bool commandMenu = false;
int command = 0;

// Sending variables
bool sendMenu = false;

// Other variables
char buffer[16];

// Utilities
void printCentered(const char *text, int y = 0)
{
  // Imprimir en la mitad de la pantalla
  int x = (16 / 2) - 1;
  if (strlen(text) % 2 == 0)
  {
    x++;
  }
  x -= strlen(text) / 2;
  lcd.setCursor(x, y);
  lcd.printf(text);
}

// Main code
void setup()
{
  // Setup LCD
  lcd.begin(16, 2);

  // Setup serial
  Serial.begin(115200);

  // Setup GPIOs
  pinMode(CANCEL_PIN, INPUT);
  pinMode(OK_PIN, INPUT);
  pinMode(ADDRESS_PIN, INPUT);
  pinMode(DIM_PIN, INPUT);
  pinMode(COMMAND_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
}

void changeAddress()
{
  if (addressIndex >= 4)
  {
    addressIndex = 0;
  }

  addressType = addresses[addressIndex];
  addressIndex++;
}

void addressScreen()
{
  if (addressBtn == HIGH) {
    Serial.println("Changing address");
    changeAddress();
  }

  lcd.clear();
  switch (addressType)
  {
  case 'I':
  {
    printCentered("Ballast");
    aux = map(analog, 0, 4095, 0, 63);
    sprintf(buffer, "%d", aux);
    printCentered(buffer, 1);

    break;
  }

  case 'G':
  {
    printCentered("Group");
    aux = map(analog, 0, 4095, 1, 15);
    sprintf(buffer, "%d", aux);
    printCentered(buffer, 1);

    break;
  }

  case 'B':
  {
    printCentered("Broadcast");
    printCentered("##", 1);

    break;
  }

  case 'D':
  {
    printCentered("DTR Address");
    aux = map(analog, 0, 4095, 1, 63);
    sprintf(buffer, "%d", aux);
    printCentered(buffer, 1);

    break;
  }

  default:
    Serial.println("Error, unknown address type");
    break;
  };

  // If the type address menu is open, save the selected address in memory
  if (cancelBtn == HIGH)
  {
    addressMenu = false;
    addressIndex = prevAddressIndex - 1;
    addressType = prevAddressType;
    delay(500);
    return;
  }

  if (okBtn == HIGH)
  {
    address = aux;
    addressMenu = false;
    prevAddressIndex = addressIndex;
    prevAddressType = addressType;
    addressIndex--;
    delay(500);
    return;
  }


}

void dimScreen()
{
  lcd.clear();

  if (addressType == 'D')
  {
    printCentered("DTR Value");
    aux = map(analog, 0, 4095, 0, 254);
    sprintf(buffer, "%d", aux);
    if (aux == 0 && addressType != 'D') {
      sprintf(buffer, "%s", "0 (OFF)");
    }
      
    printCentered(buffer, 1);

    // Check buttons
    if (cancelBtn == HIGH)
    {
      dimMenu = false;
      dimRGB = false;
      startDimChanging = false;
      sendRGB = false;
      delay(500);
      return;
    }

    if (okBtn == HIGH)
    {
      bright = aux;
      dimRGB = false;
      startDimChanging = false;
      dimMenu = false;
      sendRGB = false;
      delay(500);
      return;
    }
  }
  else if (!dimRGB && startDimChanging) {
    printCentered("Bright Level:");
    aux = map(analog, 0, 4095, 0, 254);
    sprintf(buffer, "%d", aux);
    if (aux == 0 && addressType != 'D') {
      sprintf(buffer, "%s", "0 (OFF)");
    }
    printCentered(buffer, 1);

    // Check buttons
    if (cancelBtn == HIGH)
    {
      dimMenu = false;
      dimRGB = false;
      startDimChanging = false;
      delay(500);
      return;
    }

    if (okBtn == HIGH)
    {
      bright = aux;
      sendRGB = false;
      dimMenu = false;
      dimRGB = false;
      startDimChanging = false;
      delay(500);
      return;
    }
  }
  else if (dimRGB && startDimChanging) {
    aux = map(analog, 0, 4095, 0, 254);
    sprintf(buffer, "%d", aux);
    if (currentColor == 'R') {
      printCentered("Red level:");

      if (okBtn == HIGH)
      {
        red = aux;
        currentColor = 'G';
        delay(500);
        return;
      }   
    } 

    if (currentColor == 'G') {
      printCentered("Green level:");
      if (okBtn == HIGH)
      {
        green = aux;
        currentColor = 'B';
        delay(500);
        return;
      }  
    }

    if (currentColor == 'B') {
      printCentered("Blue level:");
      if (okBtn == HIGH)
      {
        blue = aux;
        currentColor = 'R';
        dimRGB = false;
        sendRGB = true;
        startDimChanging = false;
        dimMenu = false;
        delay(500);
        return;
      }  
    }

    printCentered(buffer, 1);

    if (cancelBtn == HIGH)
    {
      dimMenu = false;
      dimRGB = false;
      currentColor = 'R';
      startDimChanging = false;
      delay(500);
      return;
    }
  }
  else
  {
    printCentered("Change RGB?");
    if (cancelBtn == HIGH)
    {
      dimRGB = false;
      currentColor = 'R';
      startDimChanging = true;
      delay(500);
      return;
    }

    if (okBtn == HIGH)
    {
      dimRGB = true;
      startDimChanging = true;
      delay(500);
      return;
    }
  }




}

void commandScreen()
{
  lcd.clear();

  printCentered("Command code");
  aux = map(analog, 0, 4095, 0, 128);
  sprintf(buffer, "%X", aux);
  printCentered(buffer, 1);

  // Check buttons
  if (cancelBtn == HIGH)
  {
    commandMenu = false;
    delay(500);
    return;
  }

  if (okBtn == HIGH)
  {
    if ((aux >= 9 && aux <= 15) || (aux >= 34 && aux <= 41) || (aux >= 48 && aux <= 63)) {
      lcd.clear();
      printCentered("Reserved command");
      delay(1000);
      return;
    }

    command = aux;
    commandMenu = false;
    delay(500);
    return;
  }
}

void handleSendCode()
{
  switch (addressType)
  {
  case 'I':
    dali.prepareShortAddress(address);
    break;

  case 'G':
    dali.prepareGroupAddress(address);
    break;

  case 'B':
    dali.prepareBroadcast();
    break;

  case 'D':
    // Does automatically the storing of the current brightness selected in DTR by first dimming a lamp and then calling the 0x21 command
    Serial.printf("Setting the DTR value [%d] by using the (%d) ballast\n", bright, address);
    dali.prepareShortAddress(address);
    dali.dim(bright);
    delayMicroseconds(9200);
    dali.saveBrightLevelToDTR();
    delayMicroseconds(9200);
    dali.dim(127);
    return;

  default:
    break;
  };

  if (sendBright)
  {
    // If there's RGB selected to send, it will send three signals
    if (sendRGB) {
      dali.dimRGB(red, green, blue);
      Serial.printf("(R:%d)(G:%d)(B:%d) arc power level set at address %c:%d\n", red, green, blue, addressType, address);
    } else {
      dali.dim(bright);
      Serial.printf("(%d) arc power level set at address %c:%d\n", bright, addressType, address);
    }
    return;
  }

  // If not want to send an arc power level, the command must be selected
  switch (command)
  {
  case 0:
    dali.switchOff();
    Serial.printf("0x00 [switchOff] at address %c:%d\n", addressType, address);
    break;

  case 1:
    dali.turnBrighterWithStepSpeed();
    Serial.printf("0x01 [turnBrighterWithStepSpeed] at address %c:%d\n", addressType, address);
    break;

  case 2:
    dali.turnDarkerWithStepSpeed();
    Serial.printf("0x02 [turnDarkerWithStepSpeed] at address %c:%d\n", addressType, address);
    break;

  case 3:
    dali.increasePowerValueByStepBounded();
    Serial.printf("0x03 [increasePowerValueByStepBounded] at address %c:%d\n", addressType, address);
    break;

  case 4:
    dali.decreasePowerValueByStepBounded();
    Serial.printf("0x04 [decreasePowerValueByStepBounded] at address %c:%d\n", addressType, address);
    break;

  case 5:
    dali.setPowerToMaximum();
    Serial.printf("0x05 [setPowerToMaximum] at address %c:%d\n", addressType, address);
    break;

  case 6:
    dali.setPowerToMinimum();
    Serial.printf("0x06 [setPowerToMinimum] at address %c:%d\n", addressType, address);
    break;

  case 7:
    dali.decreasePowerValueByStepUnbounded();
    Serial.printf("0x07 [decreasePowerValueByStepUnbounded] at address %c:%d\n", addressType, address);
    break;

  case 8:
    dali.increasePowerValueByStepUnbounded();
    Serial.printf("0x08 [increasePowerValueByStepUnbounded] at address %c:%d\n", addressType, address);
    break;

  case (0x20):
    dali.resetParameters();
    Serial.printf("0x20 [resetParameters] at address %c:%d\n", addressType, address);
    break;

  case (0x21):
    dali.saveBrightLevelToDTR();
    Serial.printf("0x21 [saveBrightLevelToDTR] at address %c:%d\n", addressType, address);
    break;

  case (0x2A):
    dali.saveDTRAsMaximumPowerValue();
    Serial.printf("0x2A [saveDTRAsMaximumPowerValue] at address %c:%d\n", addressType, address);
    break;

  case (0x2B):
    dali.saveDTRAsMinimumPowerValue();
    Serial.printf("0x2B [saveDTRAsMinimumPowerValue] at address %c:%d\n", addressType, address);
    break;

  case (0x2C):
    dali.saveDTRAsFaultConditionPowerValue();
    Serial.printf("0x2C [saveDTRAsFaultConditionPowerValue] at address %c:%d\n", addressType, address);
    break;

  case (0x2D):
    dali.saveDTRAsSwitchOnPowerValue();
    Serial.printf("0x2D [saveDTRAsSwitchOnPowerValue] at address %c:%d\n", addressType, address);
    break;

  case (0x2E):
    dali.saveDTRAsStepTime();
    Serial.printf("0x2E [saveDTRAsStepTime] at address %c:%d\n", addressType, address);
    break;

  case (0x2F):
    dali.saveDTRAsStepSpeed();
    Serial.printf("0x2F [saveDTRAsStepSpeed] at address %c:%d\n", addressType, address);
    break;

  case (0x80):
    dali.saveDTRValueAsShortAddress();
    Serial.printf("0x80 [saveDTRValueAsShortAddress] at address %c:%d\n", addressType, address);
    break;

  default:
    // Handle the values with ranges
    if (command >= 16 && command <= 31) {
      Serial.printf("0x%X [setStoredPowerScene] at address %c:%d\n", command, addressType, address);
      dali.setStoredPowerScene(command);
    };

    if (command >= 64 && command <= 79) {
      Serial.printf("0x%X [saveDTRAsSceneBrightness] at address %c:%d\n", command, addressType, address);
      dali.saveDTRAsSceneBrightness(command);
    };

    if (command >= 80 && command <= 95) {
      Serial.printf("0x%X [deleteSelectedScene] at address %c:%d\n", command, addressType, address);
      dali.deleteSelectedScene(command);
    };

    if (command >= 96 && command <= 111) {
      Serial.printf("0x%X [addToGroup] at address %c:%d\n", command, addressType, address);
      dali.addToGroup(command);
    };
  
    if (command >= 112 && command <= 127) {
      Serial.printf("0x%X [deleteOfGroup] at address %c:%d\n", command, addressType, address);
      dali.deleteOfGroup(command);
    };

    // Handle reserved commands
    if ((command >= 9 && command <= 15) || (command >= 34 && command <= 41) || (command >= 48 && command <= 63)) {
      Serial.printf("(0x%X) Error during sending, reserved command\n", command);
    }

    break;
  };
}

void sendScreen()
{
  printCentered("Are you sure?");
  printCentered("Press OK to send", 1);

  if (okBtn == HIGH)
  {
    // Send manchester code
    lcd.clear();
    printCentered("Sending code");
    sendMenu = false;
    handleSendCode();
    delay(1000);
    return;
  }

  if (cancelBtn == HIGH)
  {
    sendMenu = false;
    delay(500);
    return;
  }
}

void loop()
{
  lcd.clear();
  // Read push buttons
  cancelBtn = digitalRead(CANCEL_PIN);
  okBtn = digitalRead(OK_PIN);
  addressBtn = digitalRead(ADDRESS_PIN);
  dimBtn = digitalRead(DIM_PIN);
  commandBtn = digitalRead(COMMAND_PIN);

  // Read analog input
  analog = analogRead(POT_PIN);

  // Check for push buttons
  if (addressBtn == HIGH)
  {
    addressMenu = true;
    delay(250);
  }
  else if (dimBtn == HIGH)
  {
    // Change to bright section
    dimMenu = true;
    sendBright = true;
  }
  else if (commandBtn == HIGH)
  {
    if (addressType == 'D') {
      Serial.println("Cannot send a command in DTR value mode");
      printCentered("Not allowed");
      delay(1000);
      return;
    }

    // Change to command section
    commandMenu = true;
    sendBright = false;
  }
  else if (okBtn == HIGH && !commandMenu && !dimMenu && !addressMenu && !sendMenu)
  {
    // Go to send menu
    sendMenu = true;
    printCentered("Are you sure?");
    printCentered("Press OK to send", 1);
    delay(1000);
    return;
  }
  else if (cancelBtn == HIGH && !commandMenu && !dimMenu && !addressMenu)
  {
    // Do nothing
    Serial.printf("Address: %d\n", address);
    Serial.printf("Type address: %c \n", addressType);
    Serial.printf("Dim level: %d\n", bright);
    Serial.printf("Command: %X\n", command);
    Serial.printf("S: %s\n", sendBright ? "true" : "false");
  }



  if (addressMenu)
  {
    addressScreen();
    if (addressBtn == HIGH)
    {
      delay(250);
    }
  }
  else if (dimMenu)
  {
    dimScreen();
    if (dimBtn == HIGH)
    {
      delay(250);
    }
  }
  else if (commandMenu)
  {
    commandScreen();
    if (commandBtn == HIGH)
    {
      delay(250);
    }
  }
  else if (sendMenu)
  {
    sendScreen();
    if (okBtn == HIGH)
    {
      delay(250);
    }
  }
  else
  {
    printCentered("DALI");
    printCentered("CONTROLLER", 1);
  }

  // Prevent bouncing
  delay(100);
}
