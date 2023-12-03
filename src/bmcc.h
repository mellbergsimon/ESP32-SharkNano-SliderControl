#ifndef bmcc_H
#define bmcc_H

#include <sbus.h>
#include <ArduinoJson.h>

enum CameraParameter {
    REC = 0,
    Iris = 1,
    Focus = 2,
    AutoFocus = 3,
    Zoom = 4,
    ISO = 5,
    ShutterAngle = 6,
    WhiteBalance = 7,
    AudioLevel = 8,
    FrameRate = 9,
    Codec = 10
};

class BMCC {
public:
  BMCC(HardwareSerial *bus, const int8_t rxpin, const int8_t txpin,
        const bool inv);

  void updateBMCCFromJSON(char* JSON);
  void sendSBUSPacket();

  // void setFrameRate(int val);
  // void setWhiteBalance(int val);

  void printData();
private:
  CameraParameter activeParameter;
  //void sendBMCCdata();

  // bfs::SbusTx sbus_tx;
  // bfs::SbusData data;

  void resetSomeBMCCParams();

  HardwareSerial* serial;

  const char* getCameraParameterName(int parameter);

  void buildSBUSPacket();

  unsigned int SBUS_Channel_Data[18];
  byte SBUS_Current_Channel = 0;
  byte SBUS_Current_Channel_Bit = 0;
  byte SBUS_Current_Packet_Bit = 0;
  byte SBUS_Packet_Data[25];
  byte SBUS_Packet_Position = 0;

  byte SBUS_Failsafe_Active = 0;
  byte SBUS_Lost_Frame = 0;
};

#endif
