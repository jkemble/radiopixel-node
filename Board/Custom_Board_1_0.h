#pragma once

#include "Board.h"

class CustomBoardVer1_0 : public Board
{
public:
  CustomBoardVer1_0()
      : Board(SERIAL_BAUD, LED_PIN)
  {
  }
  virtual ~CustomBoardVer1_0() {}

  virtual bool CheckRecieveAppPacket(RadioPixel::Command &packet) override;

private:
  // Const Values for board setup and info
  const long SERIAL_BAUD = 9600; //for bluetooth, was 115200;
  const uint8_t LED_PIN = 9;

};