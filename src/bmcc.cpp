#include "bmcc.h"



BMCC::BMCC(HardwareSerial *bus, const int8_t rxpin, const int8_t txpin, const bool inv){
  serial = bus;
  serial->begin(100000, SERIAL_8E2, rxpin, txpin, inv);
  SBUS_Channel_Data[0] = 512;
  SBUS_Channel_Data[1] = 512;
  SBUS_Channel_Data[2] = 512;
  SBUS_Channel_Data[3] = 512;
  SBUS_Channel_Data[4] = 512;
  SBUS_Channel_Data[5] = 512;
  SBUS_Channel_Data[6] = 512;
  SBUS_Channel_Data[7] = 512;
  SBUS_Channel_Data[8] = 512;
  SBUS_Channel_Data[9] = 512;
  SBUS_Channel_Data[10] = 1024;
  SBUS_Channel_Data[11] = 1024;
  SBUS_Channel_Data[12] = 1024;
  SBUS_Channel_Data[13] = 1024;
  SBUS_Channel_Data[14] = 1024;
  SBUS_Channel_Data[15] = 1024;
  SBUS_Channel_Data[16] = 0;
  SBUS_Channel_Data[17] = 0;
  SBUS_Failsafe_Active = 1;
  SBUS_Lost_Frame = 1;

  framerate = 0;
  focus = 0;

  sendSBUSPacket();
}

void BMCC::updateBMCCFromJSON(char* JSON) {
  Serial.print("Updating BMCC: ");
  Serial.println(JSON);

  DynamicJsonDocument doc(256);  // Adjust the capacity based on your needs

  DeserializationError error = deserializeJson(doc, JSON);

  if (error == DeserializationError::Ok && doc.containsKey("BMCC")) {
    JsonObject bmccObject = doc["BMCC"];

    // Map the JSON keys to the corresponding SbusData fields
    SBUS_Channel_Data[REC] = bmccObject.containsKey("REC") ? bmccObject["REC"] : SBUS_Channel_Data[REC];
    SBUS_Channel_Data[Iris] = bmccObject.containsKey("Iris") ? bmccObject["Iris"] : SBUS_Channel_Data[Iris];
    SBUS_Channel_Data[Focus] = bmccObject.containsKey("Focus") ? bmccObject["Focus"] : SBUS_Channel_Data[Focus];
    SBUS_Channel_Data[AutoFocus] = bmccObject.containsKey("AutoFocus") ? bmccObject["AutoFocus"] : SBUS_Channel_Data[AutoFocus];
    SBUS_Channel_Data[Zoom] = bmccObject.containsKey("Zoom") ? bmccObject["Zoom"] : SBUS_Channel_Data[Zoom];
    SBUS_Channel_Data[ISO] = bmccObject.containsKey("ISO") ? bmccObject["ISO"] : SBUS_Channel_Data[ISO];
    SBUS_Channel_Data[ShutterAngle] = bmccObject.containsKey("ShutterAngle") ? bmccObject["ShutterAngle"] : SBUS_Channel_Data[ShutterAngle];
    SBUS_Channel_Data[WhiteBalance] = bmccObject.containsKey("WhiteBalance") ? bmccObject["WhiteBalance"] : SBUS_Channel_Data[WhiteBalance];
    SBUS_Channel_Data[AudioLevel] = bmccObject.containsKey("AudioLevel") ? bmccObject["AudioLevel"] : SBUS_Channel_Data[AudioLevel];
    SBUS_Channel_Data[FrameRate] = bmccObject.containsKey("FrameRate") ? bmccObject["FrameRate"] : SBUS_Channel_Data[FrameRate];
    SBUS_Channel_Data[Codec] = bmccObject.containsKey("Codec") ? bmccObject["Codec"] : SBUS_Channel_Data[Codec];
  }

  //sendBMCCdata();
  sendSBUSPacket();

  // resetSomeBMCCParams();

  
  // sendSBUSPacket();
}


void BMCC::resetSomeBMCCParams() {
  SBUS_Channel_Data[WhiteBalance] = 1024;
  SBUS_Channel_Data[FrameRate] = 1024;
  SBUS_Channel_Data[ShutterAngle] = 1024;
}

void BMCC::printData(){
  Serial.println("BMCC data:");
  for (int i = 1; i<12; i++) {
    Serial.print(getCameraParameterName(i));
    Serial.print(": ");
    Serial.println(SBUS_Channel_Data[i]);
  }
}


void BMCC::setFrameRate(int FrameRate){
  framerate = FrameRate;
}
int BMCC::getFrameRate(){
  return framerate;
}

void BMCC::setFocus(int Focus){
  focus = Focus;
}

int BMCC::getFocus(){
  return focus;
}

void BMCC::setWhiteBalance(int WhiteBalance){
  whitebalance = WhiteBalance;
}


const char* BMCC::getCameraParameterName(int parameter) {
  switch(parameter) {
    case REC: return "REC";
    case Iris: return "Iris";
    case Focus: return "Focus";
    case AutoFocus: return "AutoFocus";
    case Zoom: return "Zoom";
    case ISO: return "ISO";
    case ShutterAngle: return "ShutterAngle";
    case WhiteBalance: return "WhiteBalance";
    case AudioLevel: return "AudioLevel";
    case FrameRate: return "FrameRate";
    case Codec: return "Codec";
    default: return "UnknownParameter";
  }
}

void BMCC::sendSBUSPacket(){
  upDateSBUSPacket();
  buildSBUSPacket();
  serial->write(SBUS_Packet_Data, 25);
}

void BMCC::upDateSBUSPacket(){
  SBUS_Channel_Data[Focus] = focus;
  SBUS_Channel_Data[WhiteBalance] = whitebalance;
  SBUS_Channel_Data[FrameRate] = framerate;
}


void BMCC::buildSBUSPacket(){
  for(SBUS_Packet_Position = 0; SBUS_Packet_Position < 25; SBUS_Packet_Position++) SBUS_Packet_Data[SBUS_Packet_Position] = 0x00;  //Zero out packet data
 
  SBUS_Current_Packet_Bit = 0;
  SBUS_Packet_Position = 0;
  SBUS_Packet_Data[SBUS_Packet_Position] = 0x0F;  //Start Byte
  SBUS_Packet_Position++;
 
  for(SBUS_Current_Channel = 0; SBUS_Current_Channel < 16; SBUS_Current_Channel++)
  {
    for(SBUS_Current_Channel_Bit = 0; SBUS_Current_Channel_Bit < 11; SBUS_Current_Channel_Bit++)
    {
      if(SBUS_Current_Packet_Bit > 7)
      {
        SBUS_Current_Packet_Bit = 0;  //If we just set bit 7 in a previous step, reset the packet bit to 0 and
        SBUS_Packet_Position++;       //Move to the next packet byte
      }
      SBUS_Packet_Data[SBUS_Packet_Position] |= (((SBUS_Channel_Data[SBUS_Current_Channel]>>SBUS_Current_Channel_Bit) & 0x01)<<SBUS_Current_Packet_Bit);  //Downshift the channel data bit, then upshift it to set the packet data byte
      SBUS_Current_Packet_Bit++;
    }
  }
  if(SBUS_Channel_Data[16] > 1023) SBUS_Packet_Data[23] |= (1<<0);  //Any number above 1023 will set the digital servo bit
  if(SBUS_Channel_Data[17] > 1023) SBUS_Packet_Data[23] |= (1<<1);
  if(SBUS_Lost_Frame != 0) SBUS_Packet_Data[23] |= (1<<2);          //Any number above 0 will set the lost frame and failsafe bits
  if(SBUS_Failsafe_Active != 0) SBUS_Packet_Data[23] |= (1<<3);
  SBUS_Packet_Data[24] = 0x00;  //End byte
  delay(3);
}