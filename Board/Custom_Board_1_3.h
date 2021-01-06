#pragma once

#include "Board.h"

// Custom Board 1.3 introduces manual override buttons to swtich
// between scenes without need of a controller. 

class CustomBoardVer1_3 : public Board
{
public:
  CustomBoardVer1_3()
      : Board(SERIAL_BAUD, LED_PIN), firstButton(BUTTON1_PIN), secondButton(BUTTON2_PIN)
  {
  }

  virtual ~CustomBoardVer1_3() {}

  virtual void SetupButtons() override;

  virtual Sequence::Type CheckButtons() override;

  virtual bool CheckRecieveAppPacket(RadioPixel::Command &packet) override;

private:
  // Const Values for board setup and info
  const long SERIAL_BAUD = 9600; //for bluetooth, was 115200;
  const int LED_PIN = 9;

  // Pins used for Buttons
  const int BUTTON1_PIN = 4;
  const int BUTTON2_PIN = 5;

  // flash (board v1.3 and above)
  // --> Not used yet ?
  const int FLASH_PIN = 8;

  Button firstButton;
  Button secondButton;
};