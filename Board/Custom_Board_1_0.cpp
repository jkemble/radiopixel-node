#include "Custom_Board_1_0.h"

bool CustomBoardVer1_0::CheckRecieveAppPacket(RadioPixel::Command &packet)
{
  // if we receive a command from serial then become the controller
  int avail = Serial.available();
  if (avail >= sizeof packet)
  {
    // read the packet
    Serial.readBytes((char *)&packet, sizeof packet);
    if (avail > sizeof packet)
    {
      int extra = 0;
      while (Serial.available())
      {
        Serial.read();
        extra++;
      }
    }

    // take control
    isController = true;

    // Got a packet !
    return true;
  }

  return false;
}
