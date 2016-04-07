#include "Microchip_25AA02E48.h"

#define READ_instruction 0b00000011     // 25AA02A's read command
#define WRITE_instruction 0b00000010    // 25AA02A's write command
#define READ_STATUS_instruction  0b00000101   
#define WRITE_STATUS_instruction 0b00000001
#define WREN 6

EEPROM25AA02_Handle EEPROM25AA02_init(void *pMemory, const size_t numBytes) {
	EEPROM25AA02_Handle handle;

	if(numBytes < sizeof(EEPROM25AA02_Obj)){
		return((EEPROM25AA02_Handle)NULL);
	}

	// assign the handle
	handle = (EEPROM25AA02_Handle)pMemory;

	return(handle);
}

void EEPROM25AA02_setSpiHandle(EEPROM25AA02_Handle handle, SPI_Handle spiHandle){
	EEPROM25AA02_Obj *obj = (EEPROM25AA02_Obj *)handle;

	// initialize the serial peripheral interface object
	obj->spiHandle = spiHandle;

	return;
}


void EEPROM25AA02_setGpioHandle(EEPROM25AA02_Handle handle, GPIO_Handle gpioHandle){
	EEPROM25AA02_Obj *obj = (EEPROM25AA02_Obj *)handle;

	// initialize the gpio interface object
	obj->gpioHandle = gpioHandle;

	return;
}


void EEPROM25AA02_setGpio_CS(EEPROM25AA02_Handle handle, GPIO_Number_e gpio_CS){
	EEPROM25AA02_Obj *obj = (EEPROM25AA02_Obj *)handle;

	// set CS pin
	obj->gpio_CS = gpio_CS;

	return;
}

uint16_t EEPROM25AA02_spiTransferByte(MCP2515_Handle handle, const uint16_t data){
	EEPROM25AA02_Obj *obj = (EEPROM25AA02_Obj *)handle;
	volatile uint16_t ReadByte;

	SPI_write(obj->spiHandle, (data & 0xFF) << 8);

	while(1){
	    if(SPI_getIntFlagStatus(obj->spiHandle)==SPI_IntFlagStatus_Completed){
			ReadByte= SPI_read(obj->spiHandle);
			break;
		}
	}
	return(ReadByte);
}

//#########################################################################################

uint8_t EEPROM25AA02_readStatus(EEPROM25AA02_Handle handle) {
  uint8_t result = 0;   // result to return
  
  // take the chip select low to select the device:
  digitalWrite(_cs, LOW);
  
  SPI.transfer(READ_STATUS_instruction);
  result = SPI.transfer(0x00);
  digitalWrite(_cs, HIGH);
  
  return result;
}


uint8_t EEPROM25AA02_readRegister(EEPROM25AA02_Handle handle, uint8_t addr) {
  uint8_t result = 0;   // result to return
  // take the chip select low to select the device:
  digitalWrite(_cs, LOW);
  SPI.transfer(READ_instruction);
  // send the device the register you want to read:
  SPI.transfer(addr);
  // send a value of 0 to read the first byte returned:
  result = SPI.transfer(0x00);
  
  // take the chip select high to de-select:
  digitalWrite(_cs, HIGH);
  
  // return the result:
  return result;
}

uint8_t EEPROM25AA02_readRegister(EEPROM25AA02_Handle handle, uint8_t addr, uint8_t *buffer, int len) {
  if(buffer == 0)
    return 1;
  int index = 0;
  uint8_t result = 0;   // result to return
  // take the chip select low to select the device:
  digitalWrite(_cs, LOW);
  SPI.transfer(READ_instruction);
  // send the device the register you want to read:
  SPI.transfer(addr);
  while(len > index)
  {
    result = SPI.transfer(0x00);
    buffer[index] = result;
    index++;
  }
  
  digitalWrite(_cs, HIGH);
  
  // return the result:
  return 0;
}


void EEPROM25AA02_getEUI48(EEPROM25AA02_Handle handle, uint8_t *buffer) {
	// take the chip select low to select the device:
	digitalWrite(_cs, LOW);
	SPI.transfer(READ_instruction);
	// send the device the register you want to read:
	SPI.transfer(0xFA);
	for(uint8_t i = 0; i < 6; i++)
	{
		buffer[i] = SPI.transfer(0x00);
	}
	
	digitalWrite(_cs, HIGH);
}

void EEPROM25AA02_getEUI64(EEPROM25AA02_Handle handle, uint8_t *buffer) {
	// take the chip select low to select the device:
	digitalWrite(_cs, LOW);
	SPI.transfer(READ_instruction);
	// send the device the register you want to read:
	SPI.transfer(0xFA);
	for(uint8_t i = 0; i < 8; i++)
	{
		if(i == 3)
			buffer[i] = 0xFF;
		else if(i == 4)
			buffer[i] = 0xFE;
		else
			buffer[i] = SPI.transfer(0x00);
	}
	
	digitalWrite(_cs, HIGH);
}

void EEPROM25AA02_writeRegister(EEPROM25AA02_Handle handle, uint8_t addr, uint8_t value) {
  // take the chip select low to select the device:
  digitalWrite(_cs, LOW);
  SPI.transfer(WREN);
  digitalWrite(_cs, HIGH);
  
  digitalWrite(_cs, LOW);

  SPI.transfer(WRITE_instruction);
  SPI.transfer(addr); //Send register location
  SPI.transfer(value);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(_cs, HIGH);
}


void EEPROM25AA02_writeRegister(EEPROM25AA02_Handle handle, uint8_t addr, uint8_t *buffer, int len) {
  int index = 0;
  // take the chip select low to select the device:
  digitalWrite(_cs, LOW);
  SPI.transfer(WREN);
  digitalWrite(_cs, HIGH);

  digitalWrite(_cs, LOW);

  SPI.transfer(WRITE_instruction);
  SPI.transfer(addr); //Send register location
  while(len > index)
  {
    SPI.transfer(buffer[index]);
    index++;
  }

  // take the chip select high to de-select:
  digitalWrite(_cs, HIGH);
}
