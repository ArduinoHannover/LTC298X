#include "LTC298X.h"

// PRIVATE

/*
 * SPI helper-functions
 */
void LTC298X::beginTransaction(void) {
	SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
	digitalWrite(_cs, LOW);
}
void LTC298X::endTransaction(void) {
	SPI.endTransaction();
	digitalWrite(_cs, HIGH);
}
void LTC298X::write8(uint16_t addr, uint8_t data) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_WRITE);
	SPI.transfer16(addr);
	SPI.transfer(data);
	this->endTransaction();
}
uint8_t LTC298X::read8(uint16_t addr) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_READ);
	SPI.transfer16(addr);
	uint8_t val = SPI.transfer(0);
	this->endTransaction();
	return val;
}
void LTC298X::write24(uint16_t addr, uint32_t data) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_WRITE);
	SPI.transfer16(addr);
	SPI.transfer16(data >> 8);
	SPI.transfer(data & 0xFF);
	this->endTransaction();
}
uint32_t LTC298X::read24(uint16_t addr) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_READ);
	SPI.transfer16(addr);
	uint32_t val = (uint32_t)SPI.transfer16(0) << 8;
	val |= SPI.transfer(0);
	this->endTransaction();
	return val;
}
void LTC298X::write32(uint16_t addr, uint32_t data) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_WRITE);
	SPI.transfer16(addr);
	SPI.transfer16(data >> 16);
	SPI.transfer16(data & 0xFFFF);
	this->endTransaction();
}
uint32_t LTC298X::read32(uint16_t addr) {
	this->beginTransaction();
	SPI.transfer(LTC298X_SPI_READ);
	SPI.transfer16(addr);
	uint32_t val = SPI.transfer16(0);
	val <<= 16;
	val |= SPI.transfer16(0);
	this->endTransaction();
	return val;
}

// PUBLIC

LTC298X::LTC298X(uint8_t cs) : _cs(cs) {}

void LTC298X::begin(void) {
	digitalWrite(_cs, HIGH);
	pinMode(_cs, OUTPUT);
	SPI.begin();
}

/*
 * Returns done-Flag, which will reflect the interrupt state (isDone = true/INTERRUPT = HIGH).
 * For faster use, react on interrupt state instead of polling the register.
 */
bool LTC298X::isDone(void) {
	return read8(LTC298X_ADDR_CMD) & 0x40;
}
/*
 * Returns error/valid flags
 */
uint8_t LTC298X::getState(void) {
	return _state;
}
/*
 * Report values either in Fahrenheit or degree Celius.
 */
void LTC298X::reportFahrenheit(void) {
	uint8_t tmp = read8(LTC298X_ADDR_CONFIG_GLOB); //preserve old rejection settings
	this->write8(LTC298X_ADDR_CONFIG_GLOB, tmp | 0x04); //set B[2]
}
void LTC298X::reportCelsius(void) {
	uint8_t tmp = read8(LTC298X_ADDR_CONFIG_GLOB); //preserve old rejection settings
	this->write8(LTC298X_ADDR_CONFIG_GLOB, tmp & ~0x04); //unset B[2]
}
/*
 * Reject 60 and/or 50 Hz AC noises (75dB @ 1 ms MUX). Select single rejection for 120dB rejection.
 */
void LTC298X::reject6050Hz(void) {
	uint8_t tmp = read8(LTC298X_ADDR_CONFIG_GLOB) & ~0x03; //clear rejection bits and preserve reporting unit
	this->write8(LTC298X_ADDR_CONFIG_GLOB, tmp | LTC298X_REJECT_6050HZ); //set new rejection setting B[1:0]
}
void LTC298X::reject60Hz(void) {
	uint8_t tmp = read8(LTC298X_ADDR_CONFIG_GLOB) & ~0x03; //clear rejection bits and preserve reporting unit
	this->write8(LTC298X_ADDR_CONFIG_GLOB, tmp | LTC298X_REJECT_60HZ); //set new rejection setting B[1:0]
}
void LTC298X::reject50Hz(void) {
	uint8_t tmp = read8(LTC298X_ADDR_CONFIG_GLOB) & ~0x03; //clear rejection bits and preserve reporting unit
	this->write8(LTC298X_ADDR_CONFIG_GLOB, tmp | LTC298X_REJECT_50HZ); //set new rejection setting B[1:0]
}
/*
 * Set MUX switching delay to us * 10 µs, default is us = 100 or 1ms.
 */
void LTC298X::setMuxDelay(uint8_t us) {
	this->write8(LTC298X_ADDR_MUX_DELAY, us);
}
/*
 * Select the Channels for conversion.
 * Set channels to (1 << 5) | (1 << 1) to select channel 6 and 2
 */
bool LTC298X::selectConversionChannels(uint32_t channels) {
	if (channels >= 0x100000) return false; //invalid
	this->write32(LTC298X_ADDR_MULTIREAD, channels);
	return true;
}
/*
 * Start sampling of ADCs. INTERRUPT will go LOW while conversion. If done, it toggles HIGH.
 */
void LTC298X::beginConversion(uint8_t ch) {
	if (ch > 20 || ch == 0) return;
	this->write8(LTC298X_ADDR_CMD, LTC298X_CMD_BEGIN | ch);
}
void LTC298X::beginMultipleConversion(void) {
	this->write8(LTC298X_ADDR_CMD, LTC298X_CMD_BEGIN); //B[4:0] = 0
}
/*
 * Pause sampling
 */
void LTC298X::sleep(void) {
	this->write8(LTC298X_ADDR_CMD, LTC298X_CMD_SLEEP);
}

/*
 * Detach sensor from channel
 */
bool LTC298X::disableChannel(uint8_t ch) {
	if (ch < 1 ||
	    ch > 20
	) return false;
	this->write32(LTC298X_ADDR_CONFIG_CH1 + ch * 4, 0);
	return true;
}

bool LTC298X::setupADC(uint8_t ch, bool single_end) {
	if (ch < 2 - single_end ||
	    ch > 20
	) return false;
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, (uint32_t)LTC298X_TYPE_ADC << 27);
	return true;
}

/*
 * Alias
 */
bool LTC298X::setupDiode(uint8_t ch, bool single_end, bool measure_three, bool average, uint8_t current) {
	return this->setupDiode(ch, single_end, measure_three, average, current, 0);
}
/*
 * Setup a diode on a given channel.
 * Params:
 * ch            | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * single_end    | Differential measurement or externally grounded
 * measure_three | Three (8x, 4x, 1x current) or two (8x, 1x current) sampling cycles
 * average       | Calculate the average by last/2 + this/2 if difference is < 2°C
 * ideality      | Ideality factor, defaults to 1.03 if 0 is written.
 */
bool LTC298X::setupDiode(uint8_t ch, bool single_end, bool measure_three, bool average, uint8_t current, double ideality) {
	if (ch < 2 - single_end ||
	    ch > 20 ||
	    ideality < 0 ||
	    ideality >= 4 ||
	    current > DIODE_CURRENT_80uA
	) return false;
	//remove unnecessary casts
	uint32_t transmit = LTC298X_TYPE_DIODE;    //B[31:27]
	transmit <<= 1; transmit |= single_end;    //B[26:26]
	transmit <<= 1; transmit |= measure_three; //B[25:25]
	transmit <<= 1; transmit |= average;       //B[24:24]
	transmit <<= 2; transmit |= current;       //B[23:22]
	transmit <<= 22;transmit |= (uint32_t)(ideality * 1048576); //convert double to 2,20 fixed point fraction
	/*
	uint32_t transmit = ideality * 1048576; //convert double to 2,20 fixed point fraction
	transmit |= (uint32_t)current << 22;
	transmit |= (uint32_t)average << 24;
	transmit |= (uint32_t)measure_three << 25;
	transmit |= (uint32_t)single_end << 26;
	transmit |= (uint32_t)LTC298X_TYPE_DIODE << 27;
	*/
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}
/*
 * Setup a sensing resistor on a given channel.
 * Params:
 * ch            | Channel (2-20) to use.
 * resistance    | Resistance from 0 Ohm up to 131.072 MOhm
 */
bool LTC298X::setupSenseResistor(uint8_t ch, double resistance) {
	if (ch < 2 ||
	    ch > 20 ||
	    resistance < 0 ||
	    resistance >= 131072
	) return false;
	uint32_t transmit = LTC298X_TYPE_SENSERES; //B[31:27]
	transmit <<= 27; transmit |= (uint32_t)(resistance * 1024); //convert double to 17,10 fixed point fraction
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}

/*
 * Alias
 */
bool LTC298X::setupThermocouple(uint8_t ch, uint8_t type, bool single_end) {
	return this->setupThermocouple(ch, type, 0, single_end, false, TC_NO_COLDJUNCTION);
}
/*
 * Alias
 */
bool LTC298X::setupThermocouple(uint8_t ch, uint8_t type, uint8_t cj_ch, bool single_end) {
	return this->setupThermocouple(ch, type, cj_ch, single_end, false, TC_NO_COLDJUNCTION);
}
/*
 * Setup a thermocouple on a given channel.
 * Params:
 * ch         | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * type       | Any from LTC298X_TYPE_TC_J to LTC298X_TYPE_TC_B
 * cj_ch      | Channel (1-20) of cold junction compensation sensor
 * single_end | Differential measurement or externally grounded
 * oc_detect  | Detect open circuit (broken/unattached sensor)
 * oc_current | Current used for open circuit detection. Any from TC_CURRENT_10uA to TC_CURRENT_1mA
 */
bool LTC298X::setupThermocouple(uint8_t ch, uint8_t type, uint8_t cj_ch, bool single_end, bool oc_detect, uint8_t oc_current) {
	if (ch < 2 - single_end ||
	    ch > 20 ||
	    cj_ch > 20 ||
	    type < LTC298X_TYPE_TC_J ||
	    type > LTC298X_TYPE_TC_B
	) return false; //invalid
	uint32_t transmit = type;               //B[31:27]
	transmit <<= 5; transmit |= cj_ch;      //B[26:22]
	transmit <<= 1; transmit |= single_end; //B[21:21]
	transmit <<= 1; transmit |= oc_detect;  //B[20:20]
	transmit <<= 2; transmit |= oc_current; //B[19:18]
	//B[17:12] unused
	//B[11:00] unused for predefined thermocouple
	transmit <<= 18;
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}
/*
 * Setup a thermocouple on a given channel.
 * Params:
 * ch                | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * cj_ch             | Channel (1-20) of cold junction compensation sensor
 * single_end        | Differential measurement or externally grounded
 * oc_detect         | Detect open circuit (broken/unattached sensor)
 * oc_current        | Current used for open circuit detection. Any from TC_CURRENT_10uA to TC_CURRENT_1mA
 * mV                | Pointer to doubles of monotonically increasing voltages of the thermocouple
 * mV                | Pointer to doubles of monotonically increasing temperature of the thermocouple corresponding to the mV at same index
 * num_values        | Number of values in array
 * start_addr_offset | Start address in RAM indexed at 0
 */
bool LTC298X::setupCustomThermocouple(uint8_t ch, uint8_t cj_ch, bool single_end, bool oc_detect, uint8_t oc_current, double* mV, double* kelvin, uint8_t num_values, uint16_t start_addr_offset) {
	if (ch < 2 - single_end ||
	    ch > 20 ||
	    cj_ch > 20 ||
	    num_values < 3 ||
	    //ROM for custom data is only 384 byte wide, so we can store a max of 64 2x3 byte pairs
	    start_addr_offset + num_values > 63
	) return false; //invalid
	double old_mV = -1000;
	double old_kelvin = 0;
	for (uint8_t i = 0; i < num_values; i++) {
		if (mV[i] <= old_mV) return false; //must be greater
		if (mV[i] >= 256) return false; //must be less
		if (kelvin[i] <= old_kelvin) return false; //must be greater
		if (kelvin[i] >= 8192) return false; //must be less
		int32_t current_mV = (int32_t)(mV[i] * 16384); //mV is saved as 9,14 signed fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6, current_mV);
		//Kelvin is absolute, so unsinged, saved as 14,10 fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6 + 3, (uint32_t)(kelvin[i] * 1024));
		old_mV = mV[i];
		old_kelvin = kelvin[i];
	}
	uint32_t transmit = LTC298X_TYPE_TC_CUST;     //B[31:27]
	transmit <<= 5; transmit |= cj_ch;            //B[26:22]
	transmit <<= 1; transmit |= single_end;       //B[21:21]
	transmit <<= 1; transmit |= oc_detect;        //B[20:20]
	transmit <<= 2; transmit |= oc_current;       //B[19:18]
	transmit <<= 12;transmit |= start_addr_offset;//B[11:06]
	transmit <<= 6; transmit |= num_values - 1;   //B[05:00]
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}

/*
 * Setup a RTD on a given channel.
 * Params:
 * ch          | Channel (2-20) to use. CH - 1 is always used. CH - 2 is used on 3-wire and 4-wire RTDs. CH + 1 is used on 4-wire RTDs.
 * type        | Any from LTC298X_TYPE_PT_10 to LTC298X_TYPE_NI_120
 * sr_ch       | Channel (2-20) of sense resistor
 * wires       | Number of wires (2-5), while 5 equals to 4-wire and Kelvin Rsense
 * cs_rotation | Current source rotation (only available for > 2 wires)
 * sr_sharing  | Sense resistor sharing, internal grounding
 * current     | Current used for open circuit detection. Any from RTD_CURRENT_5uA to RTD_CURRENT_1mA
 * curve       | Can be any of RTD_CURVE_EUROPEAN, RTD_CURVE_AMERICAN, RTD_CURVE_JAPANESE, RTD_CURVE_ITS_90
 */
bool LTC298X::setupRTD(uint8_t ch, uint8_t type, uint8_t sr_ch, uint8_t wires, uint8_t mode, uint8_t current, uint8_t curve) {
	if (ch < (2 + (wires > 2)) ||
	    (ch + (wires >= 4)) > 20 ||
	    sr_ch < 2 ||
	    sr_ch > 20 ||
	    wires < 2 ||
	    wires > 5 ||
	    (wires < 4 && mode == LTC298X_MODE_CS_SR) ||
	    mode > LTC298X_MODE_CS_SR ||
	    type < LTC298X_TYPE_PT_10 ||
	    type > LTC298X_TYPE_NI_120
	) return false; //invalid
	//remove unnecessary casts
	uint32_t transmit = type;                //B[31:27]
	transmit <<= 5; transmit |= sr_ch;       //B[26:22]
	transmit <<= 2; transmit |= wires - 2;   //B[21:20]
	transmit <<= 2; transmit |= mode;        //B[19:18]
	transmit <<= 4; transmit |= current;     //B[17:14]
	transmit <<= 2; transmit |= curve;       //B[13:12]
	transmit <<= 12;                         //B[11:00] unused for predefined RTD
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}
bool LTC298X::setupCustomRTD(uint8_t ch, uint8_t sr_ch, uint8_t wires, uint8_t mode, uint8_t current, double* ohm, double* kelvin, uint8_t num_values, uint16_t start_addr_offset) {
	if (ch < (2 + (wires > 2)) ||
	    (ch + (wires == 4)) > 20 ||
	    sr_ch < 2 ||
	    sr_ch > 20 ||
	    wires < 2 ||
	    wires > 5 ||
	    (wires == 2 && mode == LTC298X_MODE_CS_SR) ||
	    mode > LTC298X_MODE_CS_SR ||
	    num_values < 3 ||
	    //ROM for custom data is only 384 byte wide, so we can store a max of 64 2x3 byte pairs
	    start_addr_offset + num_values > 63
	) return false; //invalid
	double old_ohm = 0;
	double old_kelvin = 0;
	for (uint8_t i = 0; i < num_values; i++) {
		if (ohm[i] <= old_ohm) return false; //must be greater
		if (ohm[i] >= 4096) return false; //must be less
		if (kelvin[i] <= old_kelvin) return false; //must be greater
		if (kelvin[i] >= 8192) return false; //must be less
		//Resistance is absolute, so unsigned, saved as 13,11 unsigned fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6, (uint32_t)(ohm[i] * 2048));
		//Kelvin is absolute, so unsinged, saved as 14,10 fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6 + 3, (uint32_t)(kelvin[i] * 1024));
		old_ohm = ohm[i];
		old_kelvin = kelvin[i];
	}
	uint32_t transmit = LTC298X_TYPE_RTD_CUST;    //B[31:27]
	transmit <<= 5; transmit |= sr_ch;            //B[26:22]
	transmit <<= 2; transmit |= wires-2;          //B[21:20]
	transmit <<= 2; transmit |= mode;             //B[19:18]
	transmit <<= 4; transmit |= current;          //B[17:14]
	transmit <<= 8; transmit |= start_addr_offset;//B[11:06]
	transmit <<= 6; transmit |= num_values - 1;   //B[05:00]
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}


/*
 * Setup a Thermistor on a given channel.
 * Params:
 * ch          | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * type        | Any from LTC298X_TYPE_THER_44004 to LTC298X_TYPE_THER_SPECT
 * sr_ch       | Channel (2-20) of sense resistor
 * single_end  | Differential measurement or externally grounded
 * cs_rotation | Current source rotation
 * sr_sharing  | Sense resistor sharing
 * current     | Current can be any from LTC298X_TYPE_THER_44004 to TR_CURRENT_AUTO
 */
bool LTC298X::setupThermistor(uint8_t ch, uint8_t type, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current) {
	if (ch < (2 - single_end) ||
	    ch > 20 ||
	    sr_ch < 2 ||
	    sr_ch > 20 ||
	    current < TR_CURRENT_250nA ||
	    current > TR_CURRENT_AUTO ||
	    mode > LTC298X_MODE_CS_SR ||
	    type < LTC298X_TYPE_THER_44004 ||
	    type > LTC298X_TYPE_THER_SPECT
	) return false; //invalid
	//remove unnecessary casts
	uint32_t transmit = type;                //B[31:27]
	transmit <<= 5; transmit |= sr_ch;       //B[26:22]
	transmit <<= 1; transmit |= single_end;  //B[21:21]
	transmit <<= 2; transmit |= mode;        //B[20:19]
	transmit <<= 4; transmit |= current;     //B[18:15]
	//B[14:12] unused
	//B[11:00] unused for predefined Thermistor
	transmit <<= 12;
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}

/*
 * Setup a Thermistor with Steinhart-Hart-Curve on a given channel.
 * Params:
 * ch          | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * sr_ch       | Channel (2-20) of sense resistor
 * single_end  | Differential measurement or externally grounded
 * cs_rotation | Current source rotation
 * sr_sharing  | Sense resistor sharing
 * current     | Current can be any from LTC298X_TYPE_THER_44004 to TR_CURRENT_AUTO
 * coeff       | Array of A-F Steinhart-Hart-Coefficients
 */
bool LTC298X::setupSteinhartHartThermistor(uint8_t ch, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current, float coeff[6], uint16_t start_addr_offset) {
	if (ch < (2 - single_end) ||
	    ch > 20 ||
	    sr_ch < 2 ||
	    sr_ch > 20 ||
	    current < TR_CURRENT_250nA ||
	    current > TR_CURRENT_AUTO ||
	    mode > LTC298X_MODE_CS_SR ||
	    start_addr_offset >= LTC298X_ADDR_RAM_WIDTH - 20
	) return false; //invalid
	for (uint8_t i = 0; i < 6; i++) {
		//Coefficients are stored as single precision floats
		uint32_t coefficient = *reinterpret_cast<uint32_t*>(&coeff[i]);
		this->write32(LTC298X_ADDR_RAM_START + start_addr_offset + i * 4, coefficient);
	}
	uint32_t transmit = LTC298X_TYPE_THER_STEINH; //B[31:27]
	transmit <<= 5; transmit |= sr_ch;            //B[26:22]
	transmit <<= 1; transmit |= single_end;       //B[21:21]
	transmit <<= 2; transmit |= mode;             //B[20:19]
	transmit <<= 4; transmit |= current;          //B[18:15]
	transmit <<= 10;transmit |= start_addr_offset;//B[11:06]
	transmit <<= 6; transmit |= 5;                //B[05:00] 
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}

/*
 * Setup a Thermistor on a given channel.
 * Params:
 * ch          | Channel (1-20) to use. If using differential sensing (single_end = false) ch - 1 is used too.
 * sr_ch       | Channel (2-20) of sense resistor
 * single_end  | Differential measurement or externally grounded
 * cs_rotation | Current source rotation
 * sr_sharing  | Sense resistor sharing
 * current     | Current can be any from LTC298X_TYPE_THER_44004 to TR_CURRENT_AUTO
 * coeff       | Array of A-F Steinhart-Hart-Coefficients
 */
bool LTC298X::setupCustomThermistor(uint8_t ch, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current, double* ohm, double* kelvin, uint8_t num_values, uint16_t start_addr_offset) {
	if (ch < (2 - single_end) ||
	    ch > 20 ||
	    sr_ch < 2 ||
	    sr_ch > 20 ||
	    current < TR_CURRENT_250nA ||
	    current > TR_CURRENT_AUTO ||
	    mode > LTC298X_MODE_CS_SR ||
	    num_values < 3 ||
	    //ROM for custom data is only 384 byte wide, so we can store a max of 64 2x3 byte pairs
	    start_addr_offset + num_values > 63
	) return false; //invalid
	double old_ohm = 0;
	double old_kelvin = 0;
	for (uint8_t i = 0; i < num_values; i++) {
		if (ohm[i] <= old_ohm) return false; //must be greater
		if (ohm[i] >= 524288) return false; //must be less
		if (kelvin[i] <= old_kelvin) return false; //must be greater
		if (kelvin[i] >= 8192) return false; //must be less
		//Resistance is absolute, so unsigned, saved as 20,4 unsigned fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6, (uint32_t)(ohm[i] * 16));
		//Kelvin is absolute, so unsinged, saved as 14,10 fixed point fraction
		write24(LTC298X_ADDR_RAM_START + start_addr_offset + i * 6 + 3, (uint32_t)(kelvin[i] * 1024));
		old_ohm = ohm[i];
		old_kelvin = kelvin[i];
	}
	uint32_t transmit = LTC298X_TYPE_THER_CUST;   //B[31:27]
	transmit <<= 5; transmit |= sr_ch;            //B[26:22]
	transmit <<= 1; transmit |= single_end;       //B[21:21]
	transmit <<= 2; transmit |= mode;             //B[20:19]
	transmit <<= 4; transmit |= current;          //B[18:15]
	transmit <<= 10;transmit |= start_addr_offset;//B[11:06]
	transmit <<= 6; transmit |= num_values - 1;   //B[05:00]
	this->write32(LTC298X_ADDR_CONFIG_CH1 + (ch - 1) * 4, transmit);
	return true;
}
/*
 * Read temperature from channel if available in previously set unit.
 */
double LTC298X::readTemperature(uint8_t ch) {
	if (ch > 20 || ch == 0) return NAN; //invalid, leave error register unchanged
	uint32_t val = this->read32(LTC298X_ADDR_RESULT_CH1 + (ch - 1) * 4);
	_state = val >> 24;
	int32_t fp_temp = val & 0xFFFFFF; //extract relevant bits
	if (fp_temp & 0x800000) fp_temp |= 0xFF000000; //convert from 24bit signed to 32bit signed
	return fp_temp/1024.0; //convert from 13,10 fixed point fraction
}
/*
 * Read raw ADC voltage in Range of GND - 50 mV and VDD - 300 mV.
 */
double LTC298X::readADC(uint8_t ch) {
	if (ch > 20 || ch == 0) return NAN; //invalid, leave error register unchanged
	uint32_t val = this->read32(LTC298X_ADDR_RESULT_CH1 + (ch - 1) * 4);
	_state = val >> 24;
	int32_t fp_temp = val & 0xFFFFFF; //extract relevant bits
	if (fp_temp & 0x800000) fp_temp |= 0xFF000000; //convert from 24bit signed to 32bit signed
	return fp_temp/2097152.0; //convert from 2,21 fixed point fraction
}