#pragma once

#include "Board.h"

// Specific Libraries for BlueTooth and Adafruit Board
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

class Feather32u4BlueFruit : public Board
{
public:
  Feather32u4BlueFruit()
      : Board(SERIAL_BAUD, LED_PIN), ble(NULL)
  {
  }

  virtual ~Feather32u4BlueFruit() {}

  virtual void SetupBluetooth() override;

  virtual bool CheckRecieveAppPacket(RadioPixel::Command &packet) override;

private:
  // Const Values for board setup and info
  const int BLUEFRUIT_SPI_CS = 8;
  const int BLUEFRUIT_SPI_IRQ = 7;
  const int BLUEFRUIT_SPI_RST = 4; // Optional but recommended, set to -1 if unused

  const long SERIAL_BAUD = 115200;
  const int LED_PIN = 13;

  Adafruit_BluefruitLE_SPI *ble;
};
