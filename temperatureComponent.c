#include "legato.h"
#include "interfaces.h"

#define externalLSB  0.25
#define internalLSB  0.0625


//Get the 32 bit data
uint32_t spiRead32 (uint8_t data_rx[])
{
	uint32_t data_rx32=data_rx[0];
	data_rx32 <<=8;
	for (int i=1;i<4;i++)
	{
		data_rx32 |= data_rx[i];
		if (i==3)
			break;
		else
			data_rx32 <<=8;
	}
	return data_rx32;
}// end of spiRead32

//Convert the data from 32 bit to Celcius value
float getCelcius(uint8_t data_rx[])
{
	float ret;
	uint32_t data_rx32 = spiRead32(data_rx);
	//Case: A problem occurred
	if (data_rx32 & 0x7)
	{
	    return NAN;
	 }
	//Case: the degree value is negative
	if (data_rx32 & 0x80000000)
	{
		data_rx32 = 0xFFFFC000 | ((data_rx32 >> 18) & 0x00003FFFF);
	}
	//Case: the degree value is positive
	else
	{
		data_rx32 >>=18;
	}
	ret = data_rx32 * externalLSB;
	return ret;
}//end of getCelcius

//Get internal degree
float getInternal (uint8_t data_rx[])
{
	uint32_t data_rx32 = spiRead32(data_rx);
	data_rx32 >>=4;
	float internal = data_rx32 & 0x7FF;
	//check if the degree value is negative
	if (data_rx32 & 0x800)
	{
		int16_t tmp = 0xF800 | (data_rx32 & 0x7FF);
		internal = tmp;
	}
	return internal *= internalLSB;

}

COMPONENT_INIT
{
	le_spi_DeviceHandleRef_t spiHandle;
	le_result_t res;
	uint8_t  data_rx[4]={0};
	size_t  readBufferSize=NUM_ARRAY_MEMBERS(data_rx);
	res = le_spi_Open("spidev1.0", &spiHandle);
	LE_FATAL_IF(res != LE_OK, "le_spi_Open failed with result=%s", LE_RESULT_TXT(res));
	LE_INFO("MAX31855 test");
	le_spi_Configure(spiHandle, 0, 8, 960000, 0);
	while (true)
	{
		le_result_t res2 = le_spi_WriteReadFD(spiHandle,data_rx,4,data_rx,&readBufferSize);
		LE_ASSERT(res2== LE_OK);
		//for (int i=0;i<readBufferSize;i++){}
			//LE_INFO( "%02x",data_rx[i]);
		LE_INFO("Internal temperature is: %.2f Celcius degree",getInternal(data_rx));
		if (isnan(getCelcius(data_rx)))
			LE_INFO("Something wrong with the thermocoupe!");
		else
			LE_INFO("Temperature from the thermocoupe is: %.2f Celcius degree",getCelcius(data_rx));
		LE_INFO(" ");
		sleep(1.25);
	}
}
