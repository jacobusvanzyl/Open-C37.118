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

#ifndef c37118configuration_h
#define c37118configuration_h

#include "c37118.h"
#include "c37118pmustation.h"

/**
* C37.118-2011 CONFIGURATION 2 FRAME CLASS
* Methods, Get/Set for each field to send and get configuration frame
* Note: CONFIG FRAME is a dynamic size, defined by a numbers of PMU
*/
class CONFIG_Frame : public C37118{
	public: 
	CONFIG_Frame();
		
	/**
	* SET Methods
	*/
	void TIME_BASE_set(uint32_t value);
	void NUM_PMU_set(uint16_t value);
	void DATA_RATE_set(uint16_t value);

	/**
	* GET Methods
	*/
	uint32_t TIME_BASE_get();
	uint16_t NUM_PMU_get();
	uint16_t DATA_RATE_get();
	
	/**
	* PACK/UNPACK Methods
	*/
	uint16_t pack(unsigned char **buff);
	void unpack(unsigned char *buffer);
	
	void PMUSTATION_ADD(PMU_Station *PS);
	
	PMU_Station *PMUSTATION_GETbyIDCODE(uint16_t idcode);
	
	
	//PMU UNIT LIST
	vector<PMU_Station*> pmu_station_list ;
	
	/**
	* DATA STORE
	*/	
	protected:
	uint32_t TIME_BASE;
	uint16_t NUM_PMU;
	uint16_t DATA_RATE;
};

/**
* C37.118-2011 CONFIGURATION 1 FRAME CLASS extends from CONFIG FRAME 2
* Methods, Get/Set for each field to send and get configuration frame
*/
class CONFIG_1_Frame : public CONFIG_Frame{
	
public: 
	CONFIG_1_Frame();	

};
#endif
