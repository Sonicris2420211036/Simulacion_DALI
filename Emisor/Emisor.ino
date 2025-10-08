#define DALI_TX  8
#define HALF_BIT_US 416


// --- FUNCIONES ---

// Envía un bit codificado en Manchester (1 o 0)
void sendManchesterBit(bool bitValue) {
  if (bitValue) {
    // '1' = transición alta-baja
    digitalWrite(DALI_TX, HIGH);
    delayMicroseconds(HALF_BIT_US);
    digitalWrite(DALI_TX, LOW);
    delayMicroseconds(HALF_BIT_US);
  } else {
    // '0' = transición baja-alta
    digitalWrite(DALI_TX, LOW);
    delayMicroseconds(HALF_BIT_US);
    digitalWrite(DALI_TX, HIGH);
    delayMicroseconds(HALF_BIT_US);
  }
}

// Envía una trama DALI (8 bits dirección + 8 bits comando)
void sendDALIFrame(uint8_t address, uint8_t command) {
  // Start bit (SIEMPRE '1')
  sendManchesterBit(1);

  // 8 bits de dirección
  for (int i = 7; i >= 0; i--) {
    sendManchesterBit(bitRead(address, i));
  }

  // 8 bits de comando
  for (int i = 7; i >= 0; i--) {
    sendManchesterBit(bitRead(command, i));
  }

  // Stop bits (2 bits en nivel bajo)
  digitalWrite(DALI_TX, LOW);
  delayMicroseconds(2 * HALF_BIT_US);
}

// --- CONFIGURACIÓN INICIAL ---
void setup() {
  pinMode(DALI_TX, OUTPUT);
  digitalWrite(DALI_TX, LOW);
  delay(1000);  // espera inicial
}

// --- LOOP PRINCIPAL ---
void loop() {
  // Ejemplo: Enviar comando "Encender" (ON)
  uint8_t address = 0x00;   // dirección 0
  uint8_t command = 0x07;   // comando ON (según estándar DALI)
  
  sendDALIFrame(address, command);

  delay(2000); // espera antes de enviar de nuevo
}
