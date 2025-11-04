  #ifndef DRIVER_H
  #define DRIVER_H
  #include <Arduino.h>

  class Driver {
    private:
      byte txPin;
      int transmissionRate;
      void castSignal(bool bit);
      void on();
      void off();
      void delay();

    public:
      Driver();
      Driver(byte txPin, const int transmissionRate);
      void sendManchesterAddress(bool Y, byte value, bool S);
      void sendManchesterCommand(byte value);
      void setupPin();
      void sendForwardFrame(bool Y, byte address, bool S, byte data);
  };

  #endif