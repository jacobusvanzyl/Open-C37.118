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

#ifndef c37118_h
#define c37118_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <string>
#include <string.h>
#include <vector>
#include <complex>
#include <iostream>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

/**
* Complex FLOAT type
*/
typedef std::complex<float> Complex;

/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0xAA = Frame Synchronization byte.  Start of Message Character
*/
#define A_SYNC_AA 0xAA     
// SYNC[1] Second byte
// bit[7] = Reserved
// bits[6,5,4] = Frame Type, bits[3,2,1,0] = Version Number (IEEE C35.118-2005 = 0001
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Data Frame byte.
*/
//      r000 = Data Frame
#define A_SYNC_DATA 0x01
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x11 = Header Frame byte.
*/
//      r001 = Header Frame
#define A_SYNC_HDR 0x11
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config1 Frame byte.
*/
//      r010 = Config Frame 1
#define A_SYNC_CFG1 0x21
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config2 Frame byte.
*/
//      r011 = Config Frame 2
#define A_SYNC_CFG2 0x31
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Command Frame byte.
*/
//      r100 = Command Frame
#define A_SYNC_CMD 0x41

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* PHASOR CHANNEL NAME MEASURE TYPE
* 0x00 = VOLTAGE
* 0x01 = CURRENT
*/
//PHASOR BIT CONF
enum PHUNIT_Bit {
	VOLTAGE = 0,
	CURRENT = 1,
};

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* ANALOG CHANNEL NAME MEASURE TYPE
* 0x00 = SINGLE POINT ON WAVE
* 0x01 = RMS ANALOG INPUT
* 0x02 = PEAK ANALOG INPUT
*/
//ANALOG BIT CONF
enum ANUNIT_Bit {
	SINGLE_POINT_ON_WAVE = 0,
	RMS_ANALOG_INPUT = 1,
	PEAK_ANALOG_INPUT = 2,
};

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* PMU NOMINAL FREQUENCY OPERATION
* 0x00 = 60 Hz
* 0x01 = 50 Hz
*/
// NOMINAL FREQ
enum FREQ_NOM {
	FN_60HZ = 0,
	FN_50HZ = 1,
};

using namespace std;

class C37118 {

public:

	/**
	* SET Methods
	*/
	void SYNC_set(uint16_t value);
	void FRAMESIZE_set(uint16_t value);
	void IDCODE_set(uint16_t value);
	void SOC_set(uint32_t value);
	void FRACSEC_set(uint32_t value);
	void CHK_set(uint16_t value);

	/**
	* GET Methods
	*/
	uint16_t SYNC_get();
	uint16_t FRAMESIZE_get();
	uint16_t IDCODE_get();
	uint32_t SOC_get();
	uint32_t FRACSEC_get();
	uint16_t CHK_get();

protected:
	/**
	* Compute CRC check FRAME based on C37.118-2011
	*/
	uint16_t Calc_CRC(uint8_t* sData, uint32_t iDataLen);

	uint16_t SYNC;
	uint16_t FRAMESIZE;
	uint16_t IDCODE;
	uint32_t SOC;
	uint32_t FRACSEC;
	uint16_t CHK;

};
#endif
