///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "board.h"

#include "MKL27Z644.h"

#include "CLI.h"

#include "Commands/MotorCommands.h"

#include "I2C_SLAVE_DRIVER.h"

#include "proccess_commands.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define PROCESSING_COMMAND				(1)

#define DEFAULT_STATE					(0)

#define CLI_LOOP_STATE					(2)

#define POOL_STATE						(3)

#define MACHINE_CONTROL_STATE			(4)

#define BREAK_EVENT						(3)

#define POOL_EVENT						(2)

#define LOOP_EVENT						(1)

#define MACHINE_CONTROL_EVENT			(3)

#define ENTER_KEY_EVENT					(1)

#define CONNECTION_SUCCESSFULLY			(1)

#define I2C_PROCCESS_COMMAD				(5)

#define I2C_BUFFER_LENGTH				(108)

#define WRITE_MOVMEMENT					(0x1)
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////


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

static uint8_t gabI2c_buffer[I2C_BUFFER_LENGTH];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	uint8_t bActualState = 0;

	uint8_t bThereIsAEvent = 0;

	uint8_t bEspecialEvent = 0;

	uint8_t bConnection = 0;

	uint8_t bi2cStatus = 0;

    BOARD_InitBootClocks();

    vfnCLI_init();

    vfnCLI_RefreshPrompt();

    i2c_slave_driver_init(0x7b, &gabI2c_buffer[0], I2C_BUFFER_LENGTH);

    vfninitI2CPins();

    while(1)
    {
    	bThereIsAEvent = vfnCLI_getEvent();

    	switch(bActualState)
		{
			case PROCESSING_COMMAND:
			{
				vfnCLI_processCommand();

				bEspecialEvent = vfnCLI_getEspecialEvent();

				if(bEspecialEvent == LOOP_EVENT)
				{
					bActualState = CLI_LOOP_STATE;

					bEspecialEvent = 0;
				}
				else if(bEspecialEvent == POOL_EVENT)
				{
					bActualState = POOL_STATE;

					bEspecialEvent = 0;
				}
				else if(bEspecialEvent == MACHINE_CONTROL_EVENT)
				{
					bActualState = MACHINE_CONTROL_STATE;

					bEspecialEvent = 0;
				}
				else
				{
					vfnCLI_RefreshPrompt();

					bActualState = DEFAULT_STATE;
				}

				break;
			}
			case POOL_STATE:
			{
				vfnEncoderLoop();

				if(bThereIsAEvent == BREAK_EVENT)
				{
					bThereIsAEvent = 0;

					bActualState = PROCESSING_COMMAND;
				}

				break;
			}
			case POOL_EVENT:
			{
				if(bThereIsAEvent == BREAK_EVENT)
				{
					bThereIsAEvent = 0;

					bActualState = PROCESSING_COMMAND;
				}
				else if(bThereIsAEvent == ENTER_KEY_EVENT)
				{
					bThereIsAEvent = 0;

					vfnCLI_SetZeroEncoder();

					vfnCLI_RefreshPrompt();

					bActualState = DEFAULT_STATE;
				}

				break;
			}
			case MACHINE_CONTROL_STATE:
			{
				if(bConnection == CONNECTION_SUCCESSFULLY)
				{
					vfnCLI_MachineControlTasks();
				}
				else
				{
					bConnection = bfnCLI_3WayHandshake();
				}

				break;
			}
			case I2C_PROCCESS_COMMAD:
			{
				vfnI2cProccess_command(&gabI2c_buffer[0]);

				bActualState = 0;

				break;
			}
			default:
			{
				if(bThereIsAEvent)
				{
					bThereIsAEvent = 0;

					bActualState = PROCESSING_COMMAND;
				}

				/* I2C - Control*/
		    	if(bi2cStatus)
		    	{
		    		bi2cStatus = 0;

		    		if(gabI2c_buffer[0] == WRITE_MOVMEMENT)
		    		{
		    			bActualState = I2C_PROCCESS_COMMAD;
		    		}

		    	}

			}
		}
    	/* The Encoders needs to be checked every time.*/
    	vfnMotor_encoder_task();

    	bi2cStatus = bfnGetStatusFlag();
	}
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
