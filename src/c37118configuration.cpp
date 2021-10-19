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

#include "c37118configuration.h"

/**
* PUBLIC Methods from CLASS CONFIGURATION FRAME 2, Get/Set for each field
*/

/**
* Method Constructor
* Init the 2 first byte with CONFIGURATION Frame ID
* Add Header Default 0xAA|0x31=Conf2 --> or 0x21 = Conf1
*/

CONFIG_Frame::CONFIG_Frame() {
	//Set header with RUNNING CONFIG (config mode 2)
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CFG2);
	this->NUM_PMU = 0;
	this->TIME_BASE = 0;
	this->NUM_PMU = 0;
	this->DATA_RATE = 0;
}

/**
*  Set TIME_BASE Field from Configuration Frame
*/
void CONFIG_Frame::TIME_BASE_set(uint32_t value) {
	this->TIME_BASE = value;
}
/**
*  Set NUM_PMU Field from Configuration Frame
*/
void CONFIG_Frame::NUM_PMU_set(uint16_t value) {
	this->NUM_PMU = value;
}
/**
*  Set DATA_RATE Field from Configuration Frame
*/
void CONFIG_Frame::DATA_RATE_set(uint16_t value) {
	this->DATA_RATE = value;
}


/**
*  Get TIME_BASE Field from Configuration Frame
*/
uint32_t CONFIG_Frame::TIME_BASE_get() {
	return this->TIME_BASE;
}
/**
*  Get NUM_PMU Field from Configuration Frame
*/
uint16_t CONFIG_Frame::NUM_PMU_get() {
	return this->NUM_PMU;
}


/**
* ADD a PMU_Station UNIT to PMU List and increase NUM_PMU
*/
void CONFIG_Frame::PMUSTATION_ADD(PMU_Station* PS) {
	this->pmu_station_list.push_back(PS);
	this->NUM_PMU++;
}

/**
* GET a PMU_Station UNIT from IDCODE Registered in the system
*/
PMU_Station* CONFIG_Frame::PMUSTATION_GETbyIDCODE(uint16_t idcode) {
	int32_t found = 0;
	for (uint32_t i = 0; i < this->pmu_station_list.size(); i++) {
		if (this->pmu_station_list[i]->IDCODE_get() == idcode) {
			found = i;
		}
	}

	return this->pmu_station_list[found];
}

/**
*  Get DATA_RATE Field from Configuration Frame
*/
uint16_t CONFIG_Frame::DATA_RATE_get() {
	return this->DATA_RATE;
}

/**
* Method unpack_config_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void CONFIG_Frame::unpack(unsigned char* buffer) {
	unsigned char* aux_buffer, * aux_buffer2 = nullptr;

	aux_buffer = buffer;
	this->SYNC_set(ntohs(*((uint16_t*)(aux_buffer))));
	aux_buffer += 2;
	this->FRAMESIZE_set(ntohs(*((uint16_t*)(aux_buffer))));
	aux_buffer += 2;
	this->IDCODE_set(ntohs(*((uint16_t*)(aux_buffer))));
	aux_buffer += 2;
	this->SOC_set(ntohl(*((uint32_t*)(aux_buffer))));
	aux_buffer += 4;
	this->FRACSEC_set(ntohl(*((uint32_t*)(aux_buffer))));
	aux_buffer += 4;
	this->TIME_BASE_set(ntohl(*((uint32_t*)(aux_buffer))));
	aux_buffer += 4;
	uint16_t pmu_count = (ntohs(*((uint16_t*)(aux_buffer))));
	aux_buffer += 2;
	// FOR EACH PMU STATION
	for (int32_t pos = 0; pos < pmu_count; pos++) {
		PMU_Station* PS = new PMU_Station();
		PS->STN_set(string((const char*)aux_buffer, 16));
		aux_buffer += 16;
		PS->IDCODE_set(ntohs(*(uint16_t*)(aux_buffer)));
		aux_buffer += 2;

		//FORMAT
		PS->FORMAT_set(ntohs(*(uint16_t*)(aux_buffer)));
		aux_buffer += 2;
		uint16_t phasor_n = ntohs(*(uint16_t*)(aux_buffer));
		aux_buffer += 2;
		uint16_t analog_n = ntohs(*(uint16_t*)(aux_buffer));
		aux_buffer += 2;
		uint16_t digital_n = ntohs(*(uint16_t*)(aux_buffer));
		aux_buffer += 2;

		// POITER TO JUMP CHANNELS SUB FRAME
		aux_buffer2 = aux_buffer + 16 * (phasor_n + analog_n + (16 * digital_n));

		// CHANNELS
		// PHASOR
		for (int32_t chan = 0; chan < phasor_n; chan++) {

			uint32_t factor_aux = ntohl(*(uint32_t*)(aux_buffer2));
			factor_aux &= 0x0FFFFFF;

			uint32_t type_aux = ntohl(*(uint32_t*)(aux_buffer2));
			type_aux &= 0xF000000;
			type_aux = (type_aux >> 24);

			PS->PHASOR_add(string((const char*)aux_buffer, 16), factor_aux, type_aux);
			aux_buffer += 16;
			aux_buffer2 += 4;
		}

		// ANALOG
		for (int32_t chan = 0; chan < analog_n; chan++) {
			uint32_t factor_aux = ntohl(*(uint32_t*)(aux_buffer2));
			factor_aux &= 0x0FFFFFF;

			uint32_t type_aux = ntohl(*(uint32_t*)(aux_buffer2));
			type_aux &= 0xF000000;
			type_aux = (type_aux >> 24);

			PS->ANALOG_add(string((const char*)aux_buffer, 16), factor_aux, type_aux);
			aux_buffer += 16;
			aux_buffer2 += 4;
		}
		// DIGITAL
		for (int32_t chan = 0; chan < digital_n; chan++) {
			vector<string> aux;
			for (int32_t bit = 0; bit < 16; bit++) {
				aux.push_back(string((const char*)aux_buffer, 16));
				aux_buffer += 16;
			}
			uint16_t msb_normal = ntohs(*(uint16_t*)(aux_buffer2));
			aux_buffer2 += 2;
			uint16_t lsb_valid = ntohs(*(uint16_t*)(aux_buffer2));
			aux_buffer2 += 2;
			PS->DIGITAL_add(aux, msb_normal, lsb_valid);
		}

		// FNOM
		PS->FNOM_set(ntohs(*(uint16_t*)(aux_buffer2)));
		aux_buffer2 += 2;

		// CFGCNT
		PS->CFGCNT_set(ntohs(*(uint16_t*)(aux_buffer2)));
		aux_buffer2 += 2;

		//UPDATE BUFFER POINTER
		aux_buffer = aux_buffer2;

		// ADD PMU UNIT TO LIST
		this->PMUSTATION_ADD(PS);
	}

	if (aux_buffer2 != nullptr) {
		// DATA_RATE
		this->DATA_RATE_set(ntohs(*((uint16_t*)(aux_buffer2))));
		aux_buffer2 += 2;

		// CRC (PREVIOUS COMPUTED in PACK/UNPACK Methods)		
		this->CHK_set(ntohs(*(uint16_t*)(aux_buffer2)));
	}
}

/**
* Method pack_config_buffer
* Mount a new config frame based on values store in the the class,
* special int16_t and int32_t pointers are needs to populate the buffer.
* total frame size = 24 + NUM_PMU*(26 + 16*(PHNMR+ANNMR+16*DGNMR)
*                                  + 4*PHNMR + 4*ANNMR + 4*DGNMR + 4);
*/
uint16_t CONFIG_Frame::pack(unsigned char** buff) {
	// total frame size = 24 + NUM_PMU*(26 + 16*(PHNMR+ANNMR+16*DGNMR) + 4*PHNMR + 4*ANNMR + 4*DGNMR + 4);
	uint16_t size = 24;

	// compute channel numbers foreach exists pmu
	for (int32_t i = 0; i < this->NUM_PMU_get(); i++) {
		size += 30; // 26 + 4
		size += 16 * (pmu_station_list[i]->PHNMR_get() + pmu_station_list[i]->ANNMR_get() + (16 * pmu_station_list[i]->DGNMR_get()));
		size += 4 * pmu_station_list[i]->PHNMR_get() + 4 * pmu_station_list[i]->ANNMR_get() + 4 * pmu_station_list[i]->DGNMR_get();
	}

	// set frame size
	this->FRAMESIZE_set(size);

	//pointers to buffer
	unsigned char* aux_buff;
	uint16_t* shptr;
	uint32_t* lptr;

	//buff size reserved
	*buff = (unsigned char*)malloc(this->FRAMESIZE * sizeof(char));
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
	lptr = (uint32_t*)(aux_buff);
	*lptr = htonl(this->TIME_BASE_get()); aux_buff += 4;
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->NUM_PMU_get()); aux_buff += 2;

	// For each pmu station
	for (int32_t i = 0; i < this->NUM_PMU_get(); i++) {
		char* cstr = new char[16];

		//Get name string and convert to char string
		strcpy(cstr, this->pmu_station_list[i]->STN_get().c_str());
		for (int32_t ptr = 0; ptr < 16; ptr++) {
			aux_buff[ptr] = cstr[ptr];
		}
		aux_buff += 16;
		//Get another fields
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->IDCODE_get()); aux_buff += 2;
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->FORMAT_get()); aux_buff += 2;
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->PHNMR_get()); aux_buff += 2;
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->ANNMR_get()); aux_buff += 2;
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->DGNMR_get()); aux_buff += 2;

		// Channels Name
		//Phasor
		for (int32_t j = 0; j < this->pmu_station_list[i]->PHNMR_get(); j++) {
			strcpy(cstr, this->pmu_station_list[i]->PH_NAME_get(j).c_str());
			for (int32_t ptr = 0; ptr < 16; ptr++) {
				aux_buff[ptr] = cstr[ptr];
			}
			aux_buff += 16;
		}
		//Analog
		for (int32_t j = 0; j < this->pmu_station_list[i]->ANNMR_get(); j++) {
			strcpy(cstr, this->pmu_station_list[i]->AN_NAME_get(j).c_str());
			for (int32_t ptr = 0; ptr < 16; ptr++) {
				aux_buff[ptr] = cstr[ptr];
			}
			aux_buff += 16;
		}
		//Digital
		for (int32_t j = 0; j < 16 * this->pmu_station_list[i]->DGNMR_get(); j++) {
			strcpy(cstr, this->pmu_station_list[i]->DG_NAME_get(j).c_str());
			for (int32_t ptr = 0; ptr < 16; ptr++) {
				aux_buff[ptr] = cstr[ptr];
			}
			aux_buff += 16;
		}

		//UNIT FACTOR
		//Phasor
		//cout << "PHNMR:" << this->pmu_station_list[i]->PHNMR_get() << endl;
		for (int32_t j = 0; j < this->pmu_station_list[i]->PHNMR_get(); j++) {
			lptr = (uint32_t*)(aux_buff);

			//cout <<"PHUNIT:" <<this->pmu_station_list[i]->PHUNIT_get(j) << endl;
			*lptr = htonl(this->pmu_station_list[i]->PHUNIT_get(j)); aux_buff += 4;
		}
		//Analog
		for (int32_t j = 0; j < this->pmu_station_list[i]->ANNMR_get(); j++) {
			lptr = (uint32_t*)(aux_buff);
			*lptr = htonl(this->pmu_station_list[i]->ANUNIT_get(j)); aux_buff += 4;
		}
		//Digital
		for (int32_t j = 0; j < this->pmu_station_list[i]->DGNMR_get(); j++) {
			lptr = (uint32_t*)(aux_buff);
			*lptr = htonl(this->pmu_station_list[i]->DGUNIT_get(j)); aux_buff += 4;
		}

		//FNOM
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->FNOM_get()); aux_buff += 2;

		// CFGCNT
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->pmu_station_list[i]->CFGCNT_get()); aux_buff += 2;

	}// END FOR OF PMU STATIONS

	//DATA RATE
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->DATA_RATE_get()); aux_buff += 2;

	// Compute CRC from current frame
	uint16_t crc_aux = Calc_CRC(*buff, this->FRAMESIZE_get() - 2);
	this->CHK_set(crc_aux);
	// get computed CRC 
	shptr = (uint16_t*)(aux_buff);
	*shptr = htons(this->CHK_get());

	// return # of bytes writed
	return (this->FRAMESIZE_get());
}

/**
* PUBLIC Methods from CLASS CONFIGURATION FRAME 1, Get/Set for each field
*/

/**
* Method Constructor - SAME OF CONFIGURATION FRAME 2, BUT...
* Init the 2 first byte with CONFIGURATION 1 Frame ID
* Add Header Default 0xAA|0x21 = Conf1
*/

CONFIG_1_Frame::CONFIG_1_Frame() {
	//Set header with RUNNING CONFIG (config mode 1)
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CFG1);
	this->NUM_PMU = 0;
}

