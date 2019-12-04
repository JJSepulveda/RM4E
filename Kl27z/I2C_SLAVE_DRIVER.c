/*
 * I2C_SLAVE_DRIVER.c
 *
 *  Created on: 26 nov 2019
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdint.h"

#include "MKL27Z644.h"

#include "fsl_i2c.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C0

#define I2C_SLAVE_CLK_SRC I2C0_CLK_SRC

#define I2C_SLAVE_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void i2c_slave_callback(I2C_Type *base, i2c_slave_transfer_t *xfer, void *userData);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

i2c_slave_handle_t g_s_handle;

volatile bool g_SlaveCompletionFlag = false;

uint8_t * g_slave_buff;

i2c_slave_config_t slaveConfig;

uint8_t gbI2c_data_length = 5;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfninitI2CPins(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[24] |= PORT_PCR_MUX(5);

}

void i2c_slave_driver_init(uint8_t bSlave_ADDR, uint8_t * pab_slave_buff, uint8_t blength_buffer)
{
	I2C_SlaveGetDefaultConfig(&slaveConfig);

	g_slave_buff = pab_slave_buff;

	slaveConfig.addressingMode = kI2C_Address7bit;
	slaveConfig.slaveAddress   = bSlave_ADDR;
	slaveConfig.upperAddress   = 0; /*  not used for this example */

	I2C_SlaveInit(EXAMPLE_I2C_SLAVE_BASEADDR, &slaveConfig, I2C_SLAVE_CLK_FREQ);

	memset(&g_s_handle, 0, sizeof(g_s_handle));

	I2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle, i2c_slave_callback, NULL);

	/* Set up slave transfer. */
	I2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle,
								 kI2C_SlaveCompletionEvent | kI2C_SlaveAddressMatchEvent);

}

uint8_t bfnGetStatusFlag(void)
{
	uint8_t bAnswer = 0;

	if(g_SlaveCompletionFlag)
	{
		g_SlaveCompletionFlag = 0;
		bAnswer = 1;
	}

	return bAnswer;
}

static void i2c_slave_callback(I2C_Type *base, i2c_slave_transfer_t *xfer, void *userData)
{
    switch (xfer->event)
    {
        /*  Address match event */
        case kI2C_SlaveAddressMatchEvent:
            xfer->data     = NULL;
            xfer->dataSize = 0;
            break;
        /*  Transmit request */
        case kI2C_SlaveTransmitEvent:
            /*  Update information for transmit process */
            xfer->data     = &g_slave_buff[2];
            xfer->dataSize = g_slave_buff[1];
            break;

        /*  Receive request */
        case kI2C_SlaveReceiveEvent:
            /*  Update information for received process */
            xfer->data     = g_slave_buff;
            xfer->dataSize = gbI2c_data_length;
            break;

        /*  Transfer done */
        case kI2C_SlaveCompletionEvent:
            g_SlaveCompletionFlag = true;
            xfer->data            = NULL;
            xfer->dataSize        = 0;
            break;

        default:
            g_SlaveCompletionFlag = false;
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
