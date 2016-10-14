#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>

#define TWI_ACK		0x00
#define TWI_NACK	0x01

void TWI_Start(TWI_t *twi);
void TWI_Restart(void);
void TWI_Stop(void);
uint8_t TWI_WriteByte(uint8_t data);
uint8_t TWI_WriteBytes(uint8_t *data, uint8_t length);
uint8_t TWI_ReadByte(uint8_t nack);
uint8_t TWI_ReadBytes(uint8_t *data, uint8_t length);

#endif
