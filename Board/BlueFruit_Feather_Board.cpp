#include "BlueFruit_Feather_Board.h"

void Feather32u4BlueFruit::SetupBluetooth()
{
  ble = new Adafruit_BluefruitLE_SPI(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

  if (ble)
  {
    ble->begin(false);
    ble->echo(false);
    ble->info();
    // Change advertised name to 12Bars_HatNode
    ble->println("AT+GAPDEVNAME=12Bars_HatNode");
    delay(100);
    ble->println("ATZ");
    delay(100);
    ble->setMode(BLUEFRUIT_MODE_DATA);
  }
}

bool Feather32u4BlueFruit::CheckRecieveAppPacket(RadioPixel::Command &packet)
{
  if ( static_cast<size_t>(ble->available()) >= sizeof packet)
  {
    // read the packet
    ble->readBytes((char *)&packet, sizeof packet);

    // become the controller
    isController = true;

    // Recieved a Packet !!
    return true;
  }

  return false;
}
