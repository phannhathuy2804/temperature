#include "legato.h"
#include "interfaces.h"

COMPONENT_INIT
{
	le_spi_DeviceHandleRef_t spiHandle;
	le_result_t res;
	uint8_t * data_rx=0;
	size_t * readBufferSize=0;
	res = le_spi_Open("spidev1.0", &spiHandle);
	LE_FATAL_IF(res != LE_OK, "le_spi_Open failed with result=%s", LE_RESULT_TXT(res));
	le_spi_Configure(spiHandle, 0, 8, 960000, 0);
	le_result_t res2 = le_spi_ReadHD(spiHandle,data_rx,readBufferSize);
	if (res2== LE_OK)
		LE_INFO((char*)data_rx);
}
