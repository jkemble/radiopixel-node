#include "Board.h"

void Board::SetupFMRadio()
{

  fmRadio = new RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HW, RFM69_IRQN);

  delay(10);
  if (fmRadio &&
      fmRadio->initialize(HN_FREQUENCY, HN_NODEID, HN_NETWORKID))
  {
    fmRadio->setHighPower(); //only for RFM69HW . . which all boards are for now.
    fmRadio->encrypt(HN_KEY);
    //fmRadio->promiscuous(false); // Updated library -> use spymode now
    fmRadio->spyMode(false);
  }
}

bool Board::CheckRecieveFMPacket(RadioPixel::Command &packet)
{
  if (fmRadio && fmRadio->receiveDone() &&
      fmRadio->DATALEN == sizeof(RadioPixel::Command))
  {
    // read the packet
    memcpy(&packet, (void *)fmRadio->DATA, fmRadio->DATALEN);

    // Make sure we are no longer in control if we were.
    isController = false;

    // Got a packet !!
    return true;
  }

  return false;
}

void Board::TransmitFMPacket(RadioPixel::Command &packet)
{
  if (fmRadio)
  {
    fmRadio->send(HN_NODEID, &packet, sizeof packet);
  }
}
