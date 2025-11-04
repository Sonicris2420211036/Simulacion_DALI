#include "Driver.h"
#include "Arduino.h"

Driver::Driver() {};

Driver::Driver(byte txPin, const int transmissionRate)
{
  this->txPin = txPin;
  this->transmissionRate = transmissionRate;
};

void Driver::on() {
  digitalWrite(txPin, HIGH);
}

void Driver::off() {
  digitalWrite(txPin, LOW);
}

void Driver::delay() {

  delayMicroseconds(this->transmissionRate);
}

void Driver::setupPin() {
  pinMode(this->txPin, OUTPUT);
  this->on();
}

void Driver::castSignal(bool bit) {
  if (bit)
  {
    // Enviar bit 1 (transición de bajo a alto)
    this->off();
    this->delay();
    this->on();
    this->delay();
  }
  else
  {
    // Enviar bit 0 (transición de alto a bajo)
    this->on();
    this->delay();
    this->off();
    this->delay();
  }
}

void Driver::sendManchesterAddress(bool Y, byte value, bool S) {
  byte aux = value << 2;

  // Poner el valor de Y
  this->castSignal(Y);

  for (int i = 7; i >= 2; i--)
  {
    // Extraer el bit más significativo
    bool bit = (aux >> i) & 1;
    this->castSignal(bit);
  }

  // Poner el valor de S
  this->castSignal(S);
}

void Driver::sendManchesterCommand(byte value) {
  for (int i = 7; i >= 0; i--)
  {
    // Extraer el bit más significativo
    bool bit = (value >> i) & 1;
    this->castSignal(bit);
  }
}

void Driver::sendForwardFrame(bool Y, byte address, bool S, byte data) {
    // Codificación Manchester para los bits de inicio, dirección, datos y parada
    // En esta versión simplificada, se envía cada bit como una señal de pulso
    // Enviar bit de inicio (transición de bajo a alto)
    this->off();
    this->delay();
    this->on();
    this->delay();

    // Enviar dirección (8 bits)
    this->sendManchesterAddress(Y, address, S);

    // Enviar datos (8 bits)
    this->sendManchesterCommand(data);

    // Enviar bits de parada (2 bits, transición de alto a bajo)
    this->on();
    this->delay();
    this->delay();
}