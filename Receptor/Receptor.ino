#include <LiquidCrystal.h>

// --- Configuración del LCD ---
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

// --- Configuración del receptor ---
const int RX_PIN = 2;              // Pin donde llega la señal del transmisor
const unsigned int HALF_BIT_US = 416; // Igual que en el transmisor (1200 bps)
const unsigned int BIT_TIME = 2 * HALF_BIT_US;

byte direccion = 0;
byte comando = 0;

void setup() {
  pinMode(RX_PIN, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.print("Esperando datos");
  Serial.begin(9600);
}

void loop() {
  // Detectar inicio de trama (flanco alto->bajo)
  if (esperarStartBit()) {
    direccion = recibirByte();
    comando = recibirByte();
    
    // Mostrar resultados
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dir: 0x");
    if (direccion < 0x10) lcd.print("0");
    lcd.print(direccion, HEX);

    lcd.setCursor(0, 1);
    lcd.print("Cmd: 0x");
    if (comando < 0x10) lcd.print("0");
    lcd.print(comando, HEX);

    Serial.print("Direccion: 0x");
    Serial.print(direccion, HEX);
    Serial.print("  Comando: 0x");
    Serial.println(comando, HEX);

    delay(2000);
    lcd.clear();
    lcd.print("Esperando datos");
  }
}

// ========================================================
// Esperar bit de inicio (Manchester: 1 = HIGH→LOW)
// ========================================================
bool esperarStartBit() {
  // Espera una transición de HIGH→LOW
  static int lastState = HIGH;
  while (true) {
    int state = digitalRead(RX_PIN);
    if (lastState == HIGH && state == LOW) {
      delayMicroseconds(HALF_BIT_US); // Centrar la lectura en el medio del bit
      return true;
    }
    lastState = state;
  }
}

// ========================================================
// Recibir un byte Manchester
// ========================================================
byte recibirByte() {
  byte valor = 0;
  for (int i = 7; i >= 0; i--) {
    bool firstHalf = digitalRead(RX_PIN);
    delayMicroseconds(HALF_BIT_US);
    bool secondHalf = digitalRead(RX_PIN);
    delayMicroseconds(HALF_BIT_US);

    bool bitVal;

    // Decodificación Manchester:
    // 1 → HIGH→LOW
    // 0 → LOW→HIGH
    if (firstHalf == HIGH && secondHalf == LOW)
      bitVal = 1;
    else if (firstHalf == LOW && secondHalf == HIGH)
      bitVal = 0;
    else
      bitVal = 0; // Por si hay error, lo tratamos como 0

    bitWrite(valor, i, bitVal);
  }
  return valor;
}

