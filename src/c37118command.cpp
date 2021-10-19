/**(C) Copyright 2014 Rafael Marsolla - rafamarsolla@gmail.com
 *
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "c37118command.h"

/**
* PUBLIC Methods from CLASS COMMAND FRAME, Get/Set for each field
* Note: EXTRAFRAME is a dynamic size, defined by user application
*       Size(EXTRAFRAME) = Size(FRAMESIZE) - 18 bytes
*/

/**
* Method Constructor
* Init the 2 first byte with Command Frame ID
* Add Header 0xAA|0x41
* Set the default FRAMESIZE with 18 bytes.
* If use EXTRAFRAME field, please update the FRAMESIZE too.
*/
// Constructor - Add CMD Frame Header 0xAA|0x41
CMD_Frame::CMD_Frame() {
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CMD);
	this->FRAMESIZE = 18;
	this->CMD = 0;
	this->EXTRAFRAME = 0;
}

/**
*  Set EXTRAFRAME Field from Command Frame
*/
void CMD_Frame::EXTRAFRAME_set(uint8_t* value) {
	this->EXTRAFRAME = value;
}
/**
*  Set CMD Field from Command Frame
*/
void CMD_Frame::CMD_set(uint16_t value) {
	this->CMD = value;
}

/**
*  Get CMD Field from Command Frame
*/
uint16_t CMD_Frame::CMD_get() {
	return this->CMD;
}
/**
*  Get EXTRAFRAME Field from Command Frame
*/
uint8_t* CMD_Frame::EXTRAFRAME_get() {
	return this->EXTRAFRAME;
}

/**
* Method unpack_cmd_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void CMD_Frame::unpack(uint8_t* buffer) {
	this->SYNC_set(ntohs(*((uint16_t*)(buffer))));
	this->FRAMESIZE_set(ntohs(*((uint16_t*)(buffer + 2))));
	this->IDCODE_set(ntohs(*((uint16_t*)(buffer + 4))));
	this->SOC_set(ntohl(*((uint32_t*)(buffer + 6))));
	this->FRACSEC_set(ntohl(*((uint32_t*)(buffer + 10))));
	this->CMD_set(ntohs(*((uint16_t*)(buffer + 14))));
	// EXTRAFRAME needs a special treatment, user defined data
	// uint8_t array to store the data
	int32_t ptr = 0;
	this->EXTRAFRAME = (uint8_t*)malloc(sizeof(uint8_t) * (this->FRAMESIZE_get() - 18));
	for (ptr; ptr < this->FRAMESIZE_get() - 18; ptr++) {
		this->EXTRAFRAME[ptr] = buffer[ptr + 16];
	}
	this->CHK_set(ntohs(*((uint16_t*)(buffer + (this->FRAMESIZE_get() - 2)))));
	//printf("Packet Received: %x,%x,%x,%x,%x,%x,%x\n",this->FRAMESIZE_get(),this->IDCODE_get(),this->SOC_get(),this->FRACSEC_get(),this->CMD_get(),this->EXTRAFRAME_get(),this->CHK_get());
}

/**
* Method pack_cmd_buffer
* Mount a new command frame based on values store in the class,
* special int16_t and int32_t pointers are needs to populate the buffer.
*/
uint16_t CMD_Frame::pack(uint8_t** buff) {
	uint8_t* aux_buff;
	uint16_t* shptr;
	uint32_t* lptr;
	//buff size reserved
	*buff = (uint8_t*)malloc(this->FRAMESIZE_get() * sizeof(uint8_t));
	//copy buff memory address
	aux_buff = *buff;
	//create a int16_t and int32_t pointers, and increment by byte_size(2,4...)
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->SYNC_get()); aux_buff += 2;
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->FRAMESIZE_get()); aux_buff += 2;
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->IDCODE_get()); aux_buff += 2;
	lptr = (uint32_t*)(aux_buff);
	*lptr = htonl(this->SOC_get()); aux_buff += 4;
	lptr = (uint32_t*)(aux_buff);
	*lptr = htonl(this->FRACSEC_get()); aux_buff += 4;
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->CMD_get()); aux_buff += 2;
	// EXTRAFRAME needs a special treatment, user defined data
	// uint8_t array to store the data
	int32_t ptr = 0;
	for (ptr; ptr < this->FRAMESIZE_get() - 18; ptr++) {
		aux_buff[ptr] = this->EXTRAFRAME[ptr];
	}
	aux_buff = *buff + (this->FRAMESIZE_get() - 2);
	shptr = (uint16_t*)(aux_buff);
	// Compute CRC from current frame
	uint16_t crc_aux = this->Calc_CRC(*buff, this->FRAMESIZE_get() - 2);
	this->CHK_set(crc_aux);
	*shptr = htons(this->CHK_get());
	return (this->FRAMESIZE_get());
}

