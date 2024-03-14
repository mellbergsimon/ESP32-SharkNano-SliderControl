#include "encoder.h"



encoder::encoder(int aPin, int bPin){
  
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  rotary = ESP32Encoder();

  rotary.attachHalfQuad(aPin, bPin);

  rotary.setCount(37);
  rotary.clearCount();

}

void encoder::setCounter(int newCount) {


  rotary.setCount((int64_t)newCount * 2);
}

int encoder::getCounter() {
  int value = (int32_t)rotary.getCount();

  if (value == 0) {

  } else {
    value = value / 2;
  }

  return value;
}




