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

#ifndef c37118pmustation_h
#define c37118pmustation_h

#include "c37118.h"

/**
* C37.118-2011 PMU STATION SUB-FRAME CLASS
* Methods, Get/Set for each field to populate a PMU Station
* Note: Used to map the current PMU and Others PMUs over Network
*
*/
class PMU_Station : public C37118 {

public:
	PMU_Station(string name, int16_t idcode, bool FREQ_TYPE, bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE);
	PMU_Station();

	/**
	* SET Methods
	*/
	void STN_set(string station_name);
	void IDCODE_set(int16_t code);
	void FORMAT_set(bool FREQ_TYPE, bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE);
	void FORMAT_set(uint16_t FORMAT_WORD);

	void PHNMR_set(uint16_t number);
	void ANNMR_set(uint16_t number);
	void DGNMR_set(uint16_t number);

	void PHASOR_add(string name, uint32_t factor, uint32_t type);
	void PHASOR_add(string name, uint32_t type);
	void PHASOR_VALUE_set(Complex value, uint32_t pos);


	void ANALOG_add(string name, int32_t factor, uint32_t type);
	void ANALOG_add(string name, uint32_t type);
	void ANALOG_VALUE_set(float value, uint32_t pos);

	void DIGITAL_add(vector<string> name, uint16_t normal, uint16_t valid);
	void DIGITAL_VALUE_set(bool value, uint32_t channel, uint32_t pos);
	void DIGITAL_VALUE_set(vector<bool> value, uint32_t channel);

	void FNOM_set(bool Freq_Nom);

	void CFGCNT_inc();
	void CFGCNT_set(uint16_t value);
	void STAT_set(uint16_t value);

	void FREQ_set(float value);
	void DFREQ_set(float value);

	void PHFACTOR_set(uint32_t pos, uint32_t factor);

	/**
	* GET Methods
	*/
	string STN_get();
	uint16_t IDCODE_get();
	bool FORMAT_COORD_get();
	bool FORMAT_PHASOR_TYPE_get();
	bool FORMAT_ANALOG_TYPE_get();
	bool FORMAT_FREQ_TYPE_get();
	uint16_t FORMAT_get();
	uint16_t DGUNIT_NORMAL_get(uint32_t pos);
	uint16_t DGUNIT_VALID_get(uint32_t pos);

	//Numbers of phasors, analog and digital channels
	uint16_t PHNMR_get();
	uint16_t ANNMR_get();
	uint16_t DGNMR_get();

	Complex PHASOR_VALUE_get(uint32_t pos);
	float ANALOG_VALUE_get(uint32_t pos);
	bool DIGITAL_VALUE_get(uint32_t channel, uint32_t pos);
	vector<bool> DIGITAL_VALUE_get(uint32_t channel);

	bool FNOM_get();

	uint16_t CFGCNT_get();
	uint16_t STAT_get();

	string PH_NAME_get(uint32_t pos);
	string AN_NAME_get(uint32_t pos);
	string DG_NAME_get(uint32_t pos);

	uint32_t PHUNIT_get(uint32_t pos);
	uint32_t ANUNIT_get(uint32_t pos);
	uint32_t DGUNIT_get(uint32_t pos);

	uint32_t PHFACTOR_get(uint32_t pos);
	uint32_t ANFACTOR_get(uint32_t pos);


	float FREQ_get();
	float DFREQ_get();

	/**
	* DATA STORE
	*/
protected:
	//STATION NAME
	string STN;
	uint16_t IDCODE;
	uint16_t FORMAT;
	uint16_t PHNMR;
	uint16_t ANNMR;
	uint16_t DGNMR;

	//CHANNEL NAMES
	vector<string> CHNAM_Phasor;
	vector<string> CHNAM_Analog;
	vector<string> CHNAM_Digital;

	//Channel Values
	vector<Complex> PHASOR_Values;
	vector<float> ANALOG_Values;
	vector< vector<bool> > DIGITAL_Values;

	//CHANNEL UNIT
	vector<uint32_t> PHUNIT;
	vector<uint32_t> ANUNIT;
	vector<uint32_t> DGUNIT;

	uint16_t FNOM;
	uint16_t CFGCNT;
	uint16_t STAT;

	float FREQ;
	float DFREQ;

};

#endif
