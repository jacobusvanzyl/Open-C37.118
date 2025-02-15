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

#include "c37118data.h"

/**
* PUBLIC Methods from CLASS DATA FRAME, Get/Set for each field
*/

/**
* Method Constructor
* Init the 2 first byte with DATA Frame ID
* Add Header of packet 0xAA|0x01
*/
DATA_Frame::DATA_Frame(CONFIG_Frame* cfg) {
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_DATA);
	this->associate_current_config = cfg;
}

/**
* Method pack_data_buffer
* Mount a new data frame based on values store in the the class,
* special int16_t and int32_t pointers are needs to populate the buffer.
* total frame size is different because the UNIT and Quantity of Channels are
* variable. All considered.
*/

typedef union _data {
	float f;
	char  s[4];
} myData;

uint16_t DATA_Frame::pack(uint8_t** buff) {
	myData aux_conv;

	uint16_t size = 14;
	// compute channel numbers foreach exists pmu
	for (int32_t i = 0; i < this->associate_current_config->NUM_PMU_get(); i++) {

		//STAT
		size += 2;

		//PASHOR
		// Data mode = float 32 bits
		if (this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()) {
			size += 8 * this->associate_current_config->pmu_station_list[i]->PHNMR_get();
		}
		// Data mode = Integer 16 bits
		else {
			size += 4 * this->associate_current_config->pmu_station_list[i]->PHNMR_get();
		}

		//FREQ and DFREQ
		// Data mode = float 32 bits
		if (this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()) {
			size += 4;//FREQ
			size += 4;//DFREQ
		}
		// Data mode = Integer 16 bits
		else {
			size += 2;//FREQ
			size += 2;//DFREQ
		}

		//ANALOG
		// Data mode = float 32 bits
		if (this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()) {
			size += 4 * this->associate_current_config->pmu_station_list[i]->ANNMR_get();
		}
		// Data mode = Integer 16 bits
		else {
			size += 2 * this->associate_current_config->pmu_station_list[i]->ANNMR_get();
		}

		//DIGITAL
		size += 2 * this->associate_current_config->pmu_station_list[i]->DGNMR_get();

	}// end for pmu station

	//Add CRC Empty Field
	size += 2;

	//set frame size
	this->FRAMESIZE_set(size);

	//pointers to buffer
	uint8_t* aux_buff;
	uint16_t* shptr;
	uint32_t* lptr;
	uint8_t* fptr;

	//buff size reserved
	if (*buff == NULL) {
		*buff = (uint8_t*)malloc(this->FRAMESIZE_get() * sizeof(uint8_t));
	}
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

	// For each pmu station
	for (int32_t i = 0; i < this->associate_current_config->NUM_PMU_get(); i++) {

		//Get Status Value
		shptr = (uint16_t*)(aux_buff);
		*shptr = htons(this->associate_current_config->pmu_station_list[i]->STAT_get());
		aux_buff += 2;

		//Channels Value

		//Foreach Phasors
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->PHNMR_get(); j++) {
			// Data mode = float
			if (this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()) {
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()) {
					//Float Converter Struct			
					aux_conv.f = abs(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					// Copy float to buffer
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++;
					//Float Converter Struct
					aux_conv.f = arg(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					// Copy float to buffer
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++;
				}
				// Coord Recta
				else {
					//Float Converter Struct
					aux_conv.f = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).real();
					// Copy float to buffer
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++;

					//Float Converter Struct
					aux_conv.f = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).imag();
					// Copy float to buffer
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr = (uint8_t*)(aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++;
				}
			}
			// Data mode = Integer 16 bits
			else {
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()) {
					shptr = (uint16_t*)(aux_buff);
					float aux = abs(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					uint16_t aux2 = (uint16_t)(aux * 1E5 / this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2);
					aux_buff += 2;

					shptr = (uint16_t*)(aux_buff);
					aux = arg(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					aux2 = (uint16_t)(aux * 1E4);
					*shptr = htons(aux2);
					aux_buff += 2;
				}
				// Coord Recta
				else {
					shptr = (uint16_t*)(aux_buff);
					float aux = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).real();
					uint16_t aux2 = (uint16_t)(aux * 1E5 / this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2);
					aux_buff += 2;

					shptr = (uint16_t*)(aux_buff);
					aux = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).imag();
					aux2 = (uint16_t)(aux * 1E5 / this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2);
					aux_buff += 2;
				}
			}// end data mode
		}// end phasors


		//FREQ e DFREQ
		// Data mode = float
		if (this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()) {

			aux_conv.f = this->associate_current_config->pmu_station_list[i]->FREQ_get();
			// Copy float to buffer
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[3]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[2]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[1]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[0]; aux_buff++;

			aux_conv.f = this->associate_current_config->pmu_station_list[i]->DFREQ_get();
			// Copy float to buffer
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[3]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[2]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[1]; aux_buff++;
			fptr = (uint8_t*)(aux_buff);
			*fptr = aux_conv.s[0]; aux_buff++;
		}

		// Data mode = Integer 16 bits
		else {
			shptr = (uint16_t*)(aux_buff);
			float aux = this->associate_current_config->pmu_station_list[i]->FREQ_get();
			if (this->associate_current_config->pmu_station_list[i]->FNOM_get())
				aux -= 50.0;
			else
				aux -= 60.0;

			uint16_t aux2 = (uint16_t)(aux * 1000);
			*shptr = htons(aux2);
			aux_buff += 2;

			//Scale *100
			shptr = (uint16_t*)(aux_buff);
			aux = this->associate_current_config->pmu_station_list[i]->DFREQ_get();
			aux2 = (uint16_t)(aux * 100);
			*shptr = htons(aux2);
			aux_buff += 2;
		}

		//Foreach ANALOGS
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->ANNMR_get(); j++) {
			// Data mode = float
			if (this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()) {
				aux_conv.f = this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_get(j);
				// Copy float to buffer
				fptr = (uint8_t*)(aux_buff);
				*fptr = aux_conv.s[3]; aux_buff++;
				fptr = (uint8_t*)(aux_buff);
				*fptr = aux_conv.s[2]; aux_buff++;
				fptr = (uint8_t*)(aux_buff);
				*fptr = aux_conv.s[1]; aux_buff++;
				fptr = (uint8_t*)(aux_buff);
				*fptr = aux_conv.s[0]; aux_buff++;
			}
			// Data mode = Integer 16 bits
			else {
				shptr = (uint16_t*)(aux_buff);
				float aux = this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_get(j);
				uint16_t aux2 = (uint16_t)(aux); //this->associate_current_config->pmu_station_list[i]->ANFACTOR_get(j));
				*shptr = htons(aux2);
				aux_buff += 2;

			}// end data mode
		}// end analog

		//Foreach DIGITAL
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->DGNMR_get(); j++) {
			shptr = (uint16_t*)(aux_buff);
			uint16_t aux = 0;
			for (int32_t k = 0; k < 16; k++) {
				aux |= (this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_get(j, k) << k);

			}
			shptr = (uint16_t*)(aux_buff);
			*shptr = htons(aux);
			aux_buff += 2;
		}
	}//foreach PMU STATION

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
* Method unpack_data_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void DATA_Frame::unpack(uint8_t* buffer) {
	uint8_t* aux_buffer;
	myData aux_conv1, aux_conv2;
	uint16_t aux1, aux2;
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

	// For each pmu station
	for (int32_t i = 0; i < this->associate_current_config->NUM_PMU_get(); i++) {

		//Get Status Value
		this->associate_current_config->pmu_station_list[i]->STAT_set(ntohs(*((uint16_t*)(aux_buffer))));
		aux_buffer += 2;
		//Foreach Phasors
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->PHNMR_get(); j++) {
			// Data mode = float
			if (this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()) {
				//Float Converter Struct
				aux_conv1.s[3] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[2] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[1] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[0] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;

				aux_conv2.s[3] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[2] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[1] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[0] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()) {
					//cout << "AUX1: " << aux1 << endl;
				//cout << "AUX2: " << aux2 << endl;
				//cout << "Polar ANG: " << aux_conv2.f << endl;

					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(polar((float)aux_conv1.f, (float)aux_conv2.f), j);
				}
				// Coord Recta
				else {
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(Complex(aux_conv1.f, aux_conv2.f), j);
				}
			}
			// Data mode = Integer 16 bits
			else {
				aux1 = ntohs(*((uint16_t*)(aux_buffer)));
				aux_buffer += 2;
				aux2 = ntohs(*((uint16_t*)(aux_buffer)));
				aux_buffer += 2;
				uint32_t aux, aux_b;
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()) {
					aux = (aux1 * this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(polar((float)(((float)aux) / 1E5f), (float)(((float)aux2) / 1E4f)), j);
				}
				// Coord Recta
				else {
					aux = (aux1 * this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					aux_b = (aux2 * this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(Complex((((float)aux) / 1E5f), (((float)aux_b) / 1E5f)), j);
				}


			}// end data mode
		}// end phasors


		//FREQ e DFREQ
		// Data mode = float
		if (this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()) {
			//Float Converter Struct
			aux_conv1.s[3] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[2] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[1] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[0] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[3] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[2] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[1] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[0] = (*((uint8_t*)(aux_buffer)));
			aux_buffer++;

			this->associate_current_config->pmu_station_list[i]->FREQ_set(aux_conv1.f);
			this->associate_current_config->pmu_station_list[i]->DFREQ_set(aux_conv2.f);
		}

		// Data mode = Integer 16 bits
		else {

			aux1 = ntohs(*((uint16_t*)(aux_buffer)));
			aux_buffer += 2;
			aux2 = ntohs(*((uint16_t*)(aux_buffer)));
			aux_buffer += 2;

			if (this->associate_current_config->pmu_station_list[i]->FNOM_get())
				this->associate_current_config->pmu_station_list[i]->FREQ_set(50.0f + ((float)aux1) / 1000);
			else
				this->associate_current_config->pmu_station_list[i]->FREQ_set(60.0f + ((float)aux1) / 1000);

			this->associate_current_config->pmu_station_list[i]->DFREQ_set(((float)aux2) / 100);

		}

		//Foreach ANALOGS
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->ANNMR_get(); j++) {
			// Data mode = float
			if (this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()) {
				aux_conv1.s[3] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[2] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[1] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[0] = (*((uint8_t*)(aux_buffer)));
				aux_buffer++;

				this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_set(aux_conv1.f, j);


			}
			// Data mode = Integer 16 bits
			else {
				aux1 = ntohs(*((uint16_t*)(aux_buffer)));
				aux_buffer += 2;
				this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_set(aux1, j);
			}// end data mode
		}// end analog

		//Foreach DIGITAL
		for (int32_t j = 0; j < this->associate_current_config->pmu_station_list[i]->DGNMR_get(); j++) {
			aux1 = ntohs(*((uint16_t*)(aux_buffer)));
			aux_buffer += 2;
			//cout<<"AUX: "<<aux1<<endl;
			for (int32_t k = 0; k < 16; k++) {
				if (((aux1 >> k) & 0x0001) == true) {
					this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_set(true, j, k);
				}
				else {
					this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_set(false, j, k);
				}

			}

		}

	}// foreach PMU STATION

	// CRC (PREVIOUS COMPUTED in PACK/UNPACK Methods)		
	this->CHK_set(ntohs(*(uint16_t*)(aux_buffer)));
}



