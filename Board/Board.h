#pragma once

#include <RFM69.h> //get it here: https://www.github.com/lowpowerlab/rfm69
#include <radiopixel_protocol.h>
#include "Sequence.h"
#include "Button.h"

class Board
{
public:
  Board(long serial_baud, int led_pin)
      : isController(false), fmRadio(NULL), serialBaud(serial_baud), ledPin(led_pin)
  {
  }

  virtual ~Board() {}

  virtual void SetupBluetooth(){}; // Only set up bluetooth for boards that need it
  virtual void SetupFMRadio();
  virtual void SetupButtons(){}; // Only set up buttons for boards that have them

  virtual Sequence::Type CheckButtons() { return Sequence::Type::NONE; } // Only boards with buttons can be checked.

  virtual bool CheckRecieveAppPacket(RadioPixel::Command &packet) { return false; }
  virtual bool CheckRecieveFMPacket(RadioPixel::Command &packet);
  virtual void TransmitFMPacket(RadioPixel::Command &packet);

  bool IsController() { return isController; }
  const long GetSerialBaud() { return serialBaud; }
  const int GetLedPin() { return ledPin; }

private:
  // Values Used for FM Radio. Constant across all boards. Break this out
  // If a new board requires a different setup
  const bool IS_RFM69HW = true;
  const int RFM69_RST = 11; // "A"
  const int RFM69_CS = 10;  // "B"
  const int RFM69_IRQ = 2;  // "SDA (only SDA/SCL/RX/TX have IRQ!)"
  const int RFM69_IRQN = digitalPinToInterrupt(RFM69_IRQ);

protected:
  bool isController;
  RFM69 *fmRadio; // Currently all boards using RFM69HW radio. If a new board is using
                  // RFM69W, then the SetupBoard call should behave appropriately.

  long serialBaud;
  int ledPin;
};
