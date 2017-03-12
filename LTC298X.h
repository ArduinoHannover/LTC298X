#ifndef LTC298X_H
#define LTC298X_H
#include <SPI.h>

/****************************************************

LTC298X library by Luca Zimmermann
This library is licensed under GNU GPLv2. 

Changelog:
V1.0 Initial Release

*****************************************************/

#define LTC298X_SPI_WRITE        0x02
#define LTC298X_SPI_READ         0x03

#define LTC298X_CMD_SLEEP        0x97
#define LTC298X_CMD_BEGIN        0x80

#define LTC298X_ADDR_CMD         0x000
#define LTC298X_ADDR_CONFIG_GLOB 0x0F0
#define LTC298X_ADDR_MULTIREAD   0x0F4
#define LTC298X_ADDR_MUX_DELAY   0x0FF

#define LTC298X_REJECT_6050HZ    0x00
#define LTC298X_REJECT_60HZ      0x01
#define LTC298X_REJECT_50HZ      0x02

#define LTC298X_ADDR_RAM_START   0x250
#define LTC298X_ADDR_RAM_END     0x3CA
#define LTC298X_ADDR_RAM_WIDTH   (LTC298X_ADDR_RAM_END - LTC298X_ADDR_RAM_START)

#define LTC298X_ADDR_CONFIG_CH1  0x200
#define LTC298X_ADDR_CONFIG_CH2  0x204
#define LTC298X_ADDR_CONFIG_CH3  0x208
#define LTC298X_ADDR_CONFIG_CH4  0x20C
#define LTC298X_ADDR_CONFIG_CH5  0x210
#define LTC298X_ADDR_CONFIG_CH6  0x214
#define LTC298X_ADDR_CONFIG_CH7  0x218
#define LTC298X_ADDR_CONFIG_CH8  0x21C
#define LTC298X_ADDR_CONFIG_CH9  0x220
#define LTC298X_ADDR_CONFIG_CH10 0x224
#define LTC298X_ADDR_CONFIG_CH11 0x228
#define LTC298X_ADDR_CONFIG_CH12 0x22C
#define LTC298X_ADDR_CONFIG_CH13 0x230
#define LTC298X_ADDR_CONFIG_CH14 0x234
#define LTC298X_ADDR_CONFIG_CH15 0x238
#define LTC298X_ADDR_CONFIG_CH16 0x23C
#define LTC298X_ADDR_CONFIG_CH17 0x240
#define LTC298X_ADDR_CONFIG_CH18 0x244
#define LTC298X_ADDR_CONFIG_CH19 0x248
#define LTC298X_ADDR_CONFIG_CH20 0x24C

#define LTC298X_ADDR_RESULT_CH1  0x010
#define LTC298X_ADDR_RESULT_CH2  0x014
#define LTC298X_ADDR_RESULT_CH3  0x018
#define LTC298X_ADDR_RESULT_CH4  0x01C
#define LTC298X_ADDR_RESULT_CH5  0x020
#define LTC298X_ADDR_RESULT_CH6  0x024
#define LTC298X_ADDR_RESULT_CH7  0x028
#define LTC298X_ADDR_RESULT_CH8  0x03C
#define LTC298X_ADDR_RESULT_CH9  0x030
#define LTC298X_ADDR_RESULT_CH10 0x034
#define LTC298X_ADDR_RESULT_CH11 0x038
#define LTC298X_ADDR_RESULT_CH12 0x03C
#define LTC298X_ADDR_RESULT_CH13 0x040
#define LTC298X_ADDR_RESULT_CH14 0x044
#define LTC298X_ADDR_RESULT_CH15 0x048
#define LTC298X_ADDR_RESULT_CH16 0x04C
#define LTC298X_ADDR_RESULT_CH17 0x050
#define LTC298X_ADDR_RESULT_CH18 0x054
#define LTC298X_ADDR_RESULT_CH19 0x058
#define LTC298X_ADDR_RESULT_CH20 0x05C


#define LTC298X_TYPE_TC_J        0x01
#define LTC298X_TYPE_TC_K        0x02
#define LTC298X_TYPE_TC_E        0x03
#define LTC298X_TYPE_TC_N        0x04
#define LTC298X_TYPE_TC_R        0x05
#define LTC298X_TYPE_TC_S        0x06
#define LTC298X_TYPE_TC_T        0x07
#define LTC298X_TYPE_TC_B        0x08
#define LTC298X_TYPE_TC_CUST     0x09

#define LTC298X_TYPE_PT_10       0x0A
#define LTC298X_TYPE_PT_50       0x0B
#define LTC298X_TYPE_PT_100      0x0C
#define LTC298X_TYPE_PT_200      0x0D
#define LTC298X_TYPE_PT_500      0x0E
#define LTC298X_TYPE_PT_1000     0x0F
#define LTC298X_TYPE_RTD_1000    0x10
#define LTC298X_TYPE_NI_120      0x11
#define LTC298X_TYPE_RTD_CUST    0x12

#define LTC298X_TYPE_THER_44004  0x13
#define LTC298X_TYPE_THER_44005  0x14
#define LTC298X_TYPE_THER_44007  0x15
#define LTC298X_TYPE_THER_44006  0x16
#define LTC298X_TYPE_THER_44008  0x17
#define LTC298X_TYPE_THER_YSI400 0x18
#define LTC298X_TYPE_THER_SPECT  0x19
#define LTC298X_TYPE_THER_STEINH 0x1A
#define LTC298X_TYPE_THER_CUST   0x1B

#define LTC298X_TYPE_DIODE       0x1C

#define LTC298X_TYPE_SENSERES    0x1D

#define LTC298X_TYPE_ADC         0x1E


#define LTC298X_ERR_SEN_HARDFAIL 0x80
#define LTC298X_ERR_ADC_HARDFAIL 0x40
#define LTC298X_ERR_CJ_HARDFAIL  0x20
#define LTC298X_ERR_CJ_SOFTFAIL  0x10
#define LTC298X_ERR_OVERRANGE    0x08
#define LTC298X_ERR_UNDERRANGE   0x04
#define LTC298X_ERR_OUTOFRANGE   0x02
#define LTC298X_ERR_CONFIG       0xFF

#define DIODE_CURRENT_10uA       0x00
#define DIODE_CURRENT_20uA       0x01
#define DIODE_CURRENT_40uA       0x02
#define DIODE_CURRENT_80uA       0x03

#define TC_CURRENT_10uA          0x00
#define TC_CURRENT_100uA         0x01
#define TC_CURRENT_500uA         0x02
#define TC_CURRENT_1000uA        0x03
#define TC_CURRENT_1mA           0x03
#define TC_NO_COLDJUNCTION       0x00

#define RTD_CURRENT_5uA          0x01
#define RTD_CURRENT_10uA         0x02
#define RTD_CURRENT_25uA         0x03
#define RTD_CURRENT_50uA         0x04
#define RTD_CURRENT_100uA        0x05
#define RTD_CURRENT_250uA        0x06
#define RTD_CURRENT_500uA        0x07
#define RTD_CURRENT_1000uA       0x08
#define RTD_CURRENT_1mA          0x08
#define RTD_CURVE_EUROPEAN       0x00
#define RTD_CURVE_AMERICAN       0x01
#define RTD_CURVE_JAPANESE       0x02
#define RTD_CURVE_ITS_90         0x03

#define TR_CURRENT_250nA         0x01
#define TR_CURRENT_500nA         0x02
#define TR_CURRENT_1uA           0x03
#define TR_CURRENT_5uA           0x04
#define TR_CURRENT_10uA          0x05
#define TR_CURRENT_25uA          0x06
#define TR_CURRENT_50uA          0x07
#define TR_CURRENT_100uA         0x08
#define TR_CURRENT_250uA         0x09
#define TR_CURRENT_500uA         0x0A
#define TR_CURRENT_1000uA        0x0B
#define TR_CURRENT_1mA           0x0B
#define TR_CURRENT_AUTO          0x0C

#define LTC298X_MODE_NONE        0x00
#define LTC298X_MODE_SR          0x01
#define LTC298X_MODE_CS_SR       0x02

#define LTC298X_CH1              (1 << 0)
#define LTC298X_CH2              (1 << 1)
#define LTC298X_CH3              (1 << 2)
#define LTC298X_CH4              (1 << 3)
#define LTC298X_CH5              (1 << 4)
#define LTC298X_CH6              (1 << 5)
#define LTC298X_CH7              (1 << 6)
#define LTC298X_CH8              (1 << 7)
#define LTC298X_CH9              (1 << 8)
#define LTC298X_CH10             (1 << 9)
#define LTC298X_CH11             (1 << 10)
#define LTC298X_CH12             (1 << 11)
#define LTC298X_CH13             (1 << 12)
#define LTC298X_CH14             (1 << 13)
#define LTC298X_CH15             (1 << 14)
#define LTC298X_CH16             (1 << 15)
#define LTC298X_CH17             (1 << 16)
#define LTC298X_CH18             (1 << 17)
#define LTC298X_CH19             (1 << 18)
#define LTC298X_CH20             (1 << 19)


class LTC298X {
	private:
		uint8_t _state = 0;
		uint8_t _cs;
		bool cache_active;
		void beginTransaction(void);
		void endTransaction(void);
		void write8(uint16_t addr, uint8_t data);
		uint8_t read8(uint16_t addr);
		void write24(uint16_t addr, uint32_t data);
		uint32_t read24(uint16_t addr);
		void write32(uint16_t addr, uint32_t data);
		uint32_t read32(uint16_t addr);
		
	public:
		LTC298X(uint8_t cs);
		void begin(void);
		
		bool isDone(void);
		uint8_t getState(void);
		void reportCelsius(void);
		void reportFahrenheit(void);
		void reject6050Hz(void);
		void reject60Hz(void);
		void reject50Hz(void);
		void setMuxDelay(uint8_t us);
		bool selectConversionChannels(uint32_t channels);
		void beginConversion(uint8_t ch);
		void beginMultipleConversion(void);
		void sleep(void);
		
		bool disableChannel(uint8_t ch);
		bool setupDiode(uint8_t ch, bool single_end, bool measure_three, bool average, uint8_t current);
		bool setupDiode(uint8_t ch, bool single_end, bool measure_three, bool average, uint8_t current, double ideality);
		bool setupSenseResistor(uint8_t ch, double resistance);
		bool setupThermocouple(uint8_t ch, uint8_t type, bool single_end);
		bool setupThermocouple(uint8_t ch, uint8_t type, uint8_t cj_ch, bool single_end);
		bool setupThermocouple(uint8_t ch, uint8_t type, uint8_t cj_ch, bool single_end, bool oc_detect, uint8_t oc_current);
		bool setupCustomThermocouple(uint8_t ch, uint8_t cj_ch, bool single_end, bool oc_detect, uint8_t oc_current, double* mV, double* kelvin, uint8_t num_values, uint16_t start_addr_offset);
		bool setupRTD(uint8_t ch, uint8_t type, uint8_t sr_ch, uint8_t wires, uint8_t mode, uint8_t current, uint8_t curve);
		bool setupCustomRTD(uint8_t ch, uint8_t sr_ch, uint8_t wires, uint8_t mode, uint8_t current, double* ohm, double* kelvin, uint8_t num_values, uint16_t start_addr_offset);
		bool setupThermistor(uint8_t ch, uint8_t type, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current);
		bool setupSteinhartHartThermistor(uint8_t ch, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current, float coeff[6], uint16_t start_addr_offset);
		bool setupCustomThermistor(uint8_t ch, uint8_t sr_ch, bool single_end, uint8_t mode, uint8_t current, double* ohm, double* kelvin, uint8_t num_values, uint16_t start_addr_offset);
		bool setupADC(uint8_t ch, bool single_end);
		
		double readTemperature(uint8_t ch);
		double readADC(uint8_t ch);
};

#endif //LTC298X_H