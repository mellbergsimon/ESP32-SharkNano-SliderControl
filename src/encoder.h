
#ifndef encoder_H
#define encoder_H


#include "ESP32Encoder.h"
#include "string.h"
#include "Arduino.h"


class encoder {
public:

  encoder(int aPin, int bPin);

  int getCounter();
  void setCounter(int newCount);

private:
  ESP32Encoder rotary;

};

#endif