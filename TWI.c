#include <avr/io.h>

#include "TWI.h"

TWI_t *TWI;
uint8_t IsStart;

void TWI_Start(TWI_t *twi)
{
	TWI = twi;
	IsStart = 1;
	// Ensure the bus is enabled
	TWI->MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
	// Force the bus into IDLE state
	TWI->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

void TWI_Restart(void)
{	// Issue a repeated START
	TWI->MASTER.CTRLC = TWI_MASTER_CMD_REPSTART_gc;
	IsStart = 1;
}

void TWI_Stop(void)
{	// Issue a STOP
	TWI->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
}

uint8_t TWI_WriteByte(uint8_t data)
{	// If this is the first byte,
	if (IsStart)
	{	// Clear the flag and write to ADDR
		IsStart = 0;
		TWI->MASTER.ADDR = data;	// Doesn't necessarily need to be the address,
	}								//   but this will generate a START condition.
	else
	{	// Otherwise, just write to DATA
		TWI->MASTER.DATA = data;
	}
	// Wait until the byte is shifted out
	while (!(TWI->MASTER.STATUS & (TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm)));
	// Return error status if we get NACK'ed, if arbitration is lost, or if there is a general bus error
	return ((TWI->MASTER.STATUS & (TWI_MASTER_RXACK_bm | TWI_MASTER_ARBLOST_bm | TWI_MASTER_BUSERR_bm)) == 0);
}

uint8_t TWI_WriteBytes(uint8_t *data, uint8_t length)
{	// Sanity check
	if (!data) return 0;
	// Track number of bytes
	uint8_t count = 0;
	// Transmit data
	for (; count < length; count++)
	{	// If we get NACK'ed, let the caller know
		if (!TWI_WriteByte(data[count])) return count;
	}
	// Finally, return the number of bytes written
	return count;
}

uint8_t TWI_ReadByte(uint8_t nack)
{	
	// Wait until the byte is shifted in
	while (!(TWI->MASTER.STATUS & TWI_MASTER_RIF_bm));
	
	// If we are done receiving,
	if (nack)
	{	// Set the ACKACT bit in CTRLC to indicate a NACK.
		TWI->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
	}

	// Initiate a read
	uint8_t retval = TWI->MASTER.DATA;

	if (nack == 0)
	{	// ACK and receive a byte
		TWI->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;		
	}

	// Return the read value
	return retval;
}

uint8_t TWI_ReadBytes(uint8_t *data, uint8_t length)
{	// Sanity check
	if (!data) return 1;
	// Offset length
	length--;
	// Read bytes
	for (uint8_t i = 0; i < length; i++)
	{
		data[i] = TWI_ReadByte(TWI_ACK);
	}
	// Read last byte with NACK
	data[length] = TWI_ReadByte(TWI_NACK);
	// Return success
	return 0;
}
