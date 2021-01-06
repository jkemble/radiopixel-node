#include "Custom_Board_1_3.h"

void CustomBoardVer1_3::SetupButtons()
{
  firstButton.initializeButton();
  secondButton.initializeButton();
}

bool CustomBoardVer1_3::CheckRecieveAppPacket(RadioPixel::Command &packet)
{
  // if we receive a command from serial then become the controller
  int avail = Serial.available();
  if (avail >= sizeof packet)
  {
    // read the packet
    Serial.readBytes((char *)&packet, sizeof packet);
#ifdef DEBUG
    Serial.print(F("received serial command, command "));
    Serial.print(packet.command);
    Serial.print(F(", pattern "));
    Serial.println(packet.pattern);
#endif
    if (avail > sizeof packet)
    {
      int extra = 0;
      while (Serial.available())
      {
        Serial.read();
        extra++;
      }
#ifdef DEBUG
      Serial.print(F("serial overflow "));
      Serial.print(extra);
      Serial.println(F(" bytes"));
#endif
    }

    // take control
    isController = true;

    // Got a packet !!
    return true;
  }

  return false;
}

Sequence::Type CustomBoardVer1_3::CheckButtons()
{
  Sequence::Type seqType = Sequence::Type::NONE;

  firstButton.update();
  secondButton.update();
  if (!firstButton.down && !secondButton.down)
  {
    // determine what button was hit and the resulting sequence
    if ((firstButton.duration() >= 2500) && (secondButton.duration() >= 2500))
    {
      isController = !isController;
    }
    else if (firstButton.duration())
    {
      if (isController)
      {
        seqType = Sequence::Type::ALERT;
      }
      else
      {
        seqType = Sequence::Type::RANDOM;
      }
    }
    else if (secondButton.duration())
    {
      seqType = Sequence::Type::RANDOM;
    }
    firstButton.clear();
    secondButton.clear();
  }
  return seqType;
}
