#include <LiquidCrystal.h>

#define DALI_RX 2
#define HALF_BIT_US 416

LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

uint8_t address = 0;
uint8_t command = 0;

// Función para leer bit codificado en Manchester
int readManchesterBit() {
  bool firstHalf = digitalRead(DALI_RX);
  delayMicroseconds(HALF_BIT_US);
  bool secondHalf = digitalRead(DALI_RX);

  if (firstHalf == HIGH && secondHalf == LOW) return 1;  // bit 1
  if (firstHalf == LOW && secondHalf == HIGH) return 0;  // bit 0

  return -1; // error
}

void setup() {
  pinMode(DALI_RX, INPUT);
  lcd.begin(16, 2);
  lcd.print("Receptor DALI");
  delay(1000);
  lcd.clear();
  lcd.print("Esperando...");
}

void loop() {
  // Esperar flanco de inicio
  while (digitalRead(DALI_RX) == LOW);

  // Centrar la lectura un poco más
  delayMicroseconds(HALF_BIT_US / 2);

  int bitValue;
  address = 0;
  command = 0;

  // Bit de start
  bitValue = readManchesterBit();
  if (bitValue != 1) return; // si no es '1', vuelve al inicio

  // 8 bits dirección
  for (int i = 7; i >= 0; i--) {
    bitValue = readManchesterBit();
    if (bitValue == -1) return;
    bitWrite(address, i, bitValue);
  }

  // 8 bits comando
  for (int i = 7; i >= 0; i--) {
    bitValue = readManchesterBit();
    if (bitValue == -1) return;
    bitWrite(command, i, bitValue);
  }

  // Ignora los bits de stop
  delayMicroseconds(2 * HALF_BIT_US);

  // Mostrar datos en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dir: 0x");
  if (address < 0x10) lcd.print("0");
  lcd.print(address, HEX);

  lcd.setCursor(0, 1);
  lcd.print("Cmd: 0x");
  if (command < 0x10) lcd.print("0");
  lcd.print(command, HEX);

  delay(1000);

  // Vuelve a modo “esperando”
  lcd.clear();
  lcd.print("Esperando...");
}


