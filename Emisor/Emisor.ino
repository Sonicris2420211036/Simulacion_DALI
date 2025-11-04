// ========================================================
// Transmisor DALI con Arduino UNO (solo TX, codificación Manchester)
// Adaptado por Cristian Buitrago
// ========================================================

// --- Configuración básica ---
const int txPin = 8;           // Pin de transmisión (usa un pin digital común)
const unsigned int HALF_BIT_US = 416;   // Duración de medio bit (1200 bps aprox)
// Bit completo = 833 µs aprox

void setup() {
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, HIGH);   // Línea en reposo (HIGH = inactivo)
  delay(1000);                 // Espera que el bus se estabilice
}

void loop() {
  // Ejemplo: encender luminaria dirección 3 (0x05)
  sendDALIForwardFrame(0x05, 0x06);  // ON
  delay(2000);

  // Apagar luminaria dirección 3
  sendDALIForwardFrame(0x05, 0x00);  // OFF
  delay(2000);
}

// ========================================================
// Función: Enviar trama DALI (1 start + 8 addr + 8 data + 2 stop)
// ========================================================
void sendDALIForwardFrame(byte address, byte data) {
  // --- Bit de START (siempre lógico 1 → HIGH→LOW) ---
  digitalWrite(txPin, HIGH);
  delayMicroseconds(HALF_BIT_US);
  digitalWrite(txPin, LOW);
  delayMicroseconds(HALF_BIT_US);

  // --- Dirección ---
  sendManchesterByte(address);

  // --- Datos ---
  sendManchesterByte(data);

  // --- Bits de parada (línea en reposo HIGH durante 2 bits) ---
  digitalWrite(txPin, HIGH);
  delayMicroseconds(4 * HALF_BIT_US); // 2 bits completos
}

// ========================================================
// Función: Envía un byte Manchester (DALI estándar)
// ========================================================
void sendManchesterByte(byte value) {
  for (int i = 7; i >= 0; i--) {
    bool bitVal = bitRead(value, i);

    if (bitVal) {
      // Bit 1 → transición HIGH → LOW
      digitalWrite(txPin, HIGH);
      delayMicroseconds(HALF_BIT_US);
      digitalWrite(txPin, LOW);
      delayMicroseconds(HALF_BIT_US);
    } else {
      // Bit 0 → transición LOW → HIGH
      digitalWrite(txPin, LOW);
      delayMicroseconds(HALF_BIT_US);
      digitalWrite(txPin, HIGH);
      delayMicroseconds(HALF_BIT_US);
    }
  }
}

