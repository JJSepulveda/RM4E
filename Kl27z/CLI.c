/*
 * CLI.c
 *
 *  Created on: 2 sep 2019
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GeneralFunctions.h"

#include "DriverUART.h"

#include "Commands/MotorCommands.h"

#include "SwTimers.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define AMOUNT_OF_COMMANDS							(9)

#define MAX_NUMBER_OF_CHARACTERS_FOR_ALL_COMMANDS	(16)

#define NUMBER_OF_COMMANDS							(AMOUNT_OF_COMMANDS)

#define SPACE_ASCII_CHARACTER						(32)

#define OFFSET_ASCCI_NUMBERS						(48)

#define MINIMUM_ASCII_NUMBER						(48)

#define MAXIMUM_ASCII_NUMBER						(57)

#define OPTION_OFFSET								(2)

#define NULL_CHARACTER								(0)

#define ENTER_ASCII									(13)

#define DEL_ASCII									(8)

#define MAX_NUMBER_OF_CHARATERS_FOR_PROMPT			(25)

#define INIT_POSITION_OF_PROMT						(2)

#define UP_KEY_ASCII								(27)

#define TRUE										(1)

#define UP_KEY_EVENT								(2)

#define CTRL_KEY_BREAK_EVENT						(3)

#define ENTER_KEY_EVENT								(1)

#define AMOUNT_OF_TEMP_COMMANDS						(5)

#define OVERFLOW_TEMP_BUFFER_MASK					(1)

#define IT_IS_A_REACALL_COMMAND						(2)

#define AMOUNT_OF_SHOW_OPTIONS						(3)

#define FREQUENCY_SHOW								(0)

#define PORTS_SHOW									(1)

#define COMMANDS_SHOW								(2)

#define CTRL_C_ASCII								(3)

#define ENCONDER_LOOP_EVENT							(1)

#define ENCODER_STREM_ESPCIAL_EVENT					(2)

#define MACHINE_CONTROL_EVENT						(3)

#define HANDSHAKE_TIMEOUT_MS						(10000)

#define REPLY_OF_MACHINE							(114)

#define NUMBER_OF_MOTORS							(6)
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (* vfnCommandFunction_t) (void);

typedef struct
{
	uint8_t bkeyEvent;
	uint8_t bEvent_flag;
}conf_CLI_data_handdle;

typedef struct
{
	uint8_t bCounterOfTempBuffer;
	uint8_t bCursorPointerOfTEmpBuffer;
	uint8_t bEspecialEventsFlags;
	uint8_t bMotor;
	uint16_t wEncoderTempBuffer;
}event_data_handdle;

typedef struct
{
	uint8_t bGetTimer;
	uint8_t bStatus;
}SwTimer_data_handdle;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void vfnHelpCommand(void);

static void vfnChagePrompt(void);

static void vfnRawStepsCommand(void);

static void vfnChangeFrequencyCommand(void);

static void vfnShowCommand(void);

static void vfnChangeDirectionCommand(void);

static void vfnEncoderCommand(void);

static void vfnSetHomeCommand(void);

static void vfnMachineControlCommand(void);

static uint8_t bfnReadCommand (uint8_t *pbArrayWithTheCommanad);

static uint8_t bfnGetPositionOfToken(uint8_t * pbSourceString, uint8_t * pbToken);

static void vfnCallBack_UART (uint8_t bByteOfTheUART);

static void vfnValidateUP_Key_event(void);

static void vfnPrintln(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t gabChangePromptOption[2] =
{
		"n"
};

static uint8_t gabRawStepOptions[4] =
{
		"m:s"
};

static uint8_t gabRawFrequencyOptions[2] =
{
		"f"
};

static uint8_t gabChageDirectionOptions[4] =
{
		"m:d"
};

static uint8_t gabEncoderOptions[2] =
{
		"m"
};

static uint8_t gabRawStepMSJ[9] =
{
		"\n\rStart"
};

static uint8_t gabShowTable[AMOUNT_OF_SHOW_OPTIONS][15] =
{
		"frequency",
		"ports",
		"commands"
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static conf_CLI_data_handdle gconf_Handdle;

/* Global variables for funcitons.*/
static uint8_t gbOffsetOfTheOpcion = 0;

/* UART Driver variables. */


/*temp flag, in the future i will set all flags into a same variable or a structure.*/

static uint8_t gb_CouterOfCharacters = 0;

/* En esta variable se guarda los argumentos de una función.*/

static uint8_t gabArgumentOfTheCommand[10];

/* gab_DataOfTheUART */
static uint8_t gab_CLIData[50];

/* Comandos */
static uint8_t gabCommandTable [AMOUNT_OF_COMMANDS][MAX_NUMBER_OF_CHARACTERS_FOR_ALL_COMMANDS] =
{
		"help",
		"changePrompt",
		"raw-step",
		"raw-freq",
		"show",
		"change-dir",
		"encoder",
		"set-home",
		"machine-control"
};

static vfnCommandFunction_t gavfnCommandTableFuncions[NUMBER_OF_COMMANDS] =
{
		&vfnHelpCommand,
		&vfnChagePrompt,
		&vfnRawStepsCommand,
		&vfnChangeFrequencyCommand,
		&vfnShowCommand,
		&vfnChangeDirectionCommand,
		&vfnEncoderCommand,
		&vfnSetHomeCommand,
		&vfnMachineControlCommand
};


/* MSJ, of CLI.*/
static uint8_t gab_Prompt[MAX_NUMBER_OF_CHARATERS_FOR_PROMPT] =
{
		"\n\rPrompt>"
};

static uint8_t gab_msj_Help[] =
{
		"\n\rThe commands that you can use are:\n\r\tled\t\t\tCambia el color del led que togglea\n\r\tmove-servo \t\tCambia el angulo de un servo en espesifico.\n\r\tauto-move \t\tmueve todos los motores de acuerdo a una secuencia preestablecida."
};

static uint8_t gab_msj_Help2[] =
{
		"\n\r\tShow\t\t\tMuestra la configuracion en ejecucion."
};

static uint8_t gab_MsjCommandDoesntExist[20] =
{
		"\n\rCommand not found"
};

static uint8_t gab_DownKey[2] =
{
		"\n"
};

static uint8_t gabEnterASCII[3] =
{
	"\r\n"
};

static uint8_t gabFrequency_MSJ[] =
{
		"\n\rModule value:      \n\rFrequency:      "
};

static uint8_t gabPortShow_MSJ[] =
{
		"\n\rID \tName \tPORT \tTPM \tChannel\r\n"
};

static uint8_t gabPortInformation_MSJ[20] =
{
		"port10\t\ttpm0\t\tch0\t\t"
};

static uint8_t gabWarningOptionCommandMSJ[] =
{
		"\r\nArgument doen't exist\r\nYou can try with the next options: \n\r"
};

static uint8_t gabEncoderLoopEventMSJ[] =
{
		"\r\nRaw count:      "
};

static uint8_t gabEncoderLoopHelloMSJ[] =
{
		"\n\rThe position will show when the encoder changes the position.\n\r"
};

static uint8_t gabEncoderPoolMSJ[] =
{
		"\n\rPress ENTER when you want to save the new position. If you don't want to change presss CTR+C\n\r[ENTER]:"
};

static uint8_t gabMachineControlFeedBack[2] =
{
		0,
		0
};

static uint8_t gabHello3WHMSJ[2] =
{
		'h',0
};

static uint8_t gabACK3WHMSJ[2] =
{
		'A',0
};

static uint8_t gab_tempBuffer[AMOUNT_OF_TEMP_COMMANDS][50];

static event_data_handdle gevent_handdle;

static SwTimer_data_handdle gs_handshakeTimer;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * The pointer byte CLIStatate it is for when there is a command this driver notifies to the application
 * */

void vfnCLI_init(void)
{
	vfnUART_Init(vfnCallBack_UART);

	vfnMotorDriver_init();

	vfnSwTimers_Init();

	gs_handshakeTimer.bGetTimer = bfnSwTimers_RequestTimer();
}

uint8_t vfnCLI_getEvent(void)
{
	return gconf_Handdle.bkeyEvent;
}

uint8_t vfnCLI_getEspecialEvent(void)
{
	return gconf_Handdle.bEvent_flag;
}

void vfnCLI_processCommand(void)
{
	uint8_t bNumberOfCommand = 0xff;

	gab_CLIData[gb_CouterOfCharacters] = NULL_CHARACTER;

	switch(gconf_Handdle.bkeyEvent)
	{
		case UP_KEY_EVENT:
		{
			vfnValidateUP_Key_event();

			vfnMemCopyV2(&gab_tempBuffer[gevent_handdle.bCursorPointerOfTEmpBuffer][0], &gab_CLIData[0],0);

			vfnUART_TX(&gab_DownKey[0]);

			vfnUART_TX(&gab_CLIData[0]);

			gb_CouterOfCharacters = wfnStrLen(&gab_CLIData[0]);

			/* it is needed to set the recall command flag, in order to avoid refresh prompt funciton.*/
			gevent_handdle.bEspecialEventsFlags = IT_IS_A_REACALL_COMMAND;

			break;
		}

		case ENTER_KEY_EVENT:
		{
			if(gb_CouterOfCharacters == 0)
			{
				break;
			}

			/* Here I save the command in order to allow me to recall the command a little bit fast.*/
			vfnMemCopyV2(&gab_CLIData[0], &gab_tempBuffer[gevent_handdle.bCounterOfTempBuffer][0],0);

			gevent_handdle.bCounterOfTempBuffer++;

			gevent_handdle.bCounterOfTempBuffer = (gevent_handdle.bCounterOfTempBuffer ==5)? 0:gevent_handdle.bCounterOfTempBuffer;/*cambiar de lugar*/

			gevent_handdle.bCursorPointerOfTEmpBuffer = gevent_handdle.bCounterOfTempBuffer;

			bNumberOfCommand = bfnReadCommand(&gab_CLIData[0]);

				if(bNumberOfCommand == 0xff)
				{
					/* here we fall if the command doesn't exist. */
					/* If the counter is not zero it is means that is a wrong command. */
					vfnUART_TX(&gab_MsjCommandDoesntExist[0]);
				}
				else
				{
					gavfnCommandTableFuncions[bNumberOfCommand]();
				}

			/* Reset variables.*/
			gb_CouterOfCharacters = 0;

			break;
		}
		case CTRL_KEY_BREAK_EVENT:
		{
			/* Reset variables.*/
			gconf_Handdle.bEvent_flag = 0;
			gb_CouterOfCharacters = 0;
			break;
		}

		default:
		{
			/* Error ?*/
		}
	}
	/* Reset flags.*/
	gconf_Handdle.bkeyEvent = 0;
}

static uint8_t bfnReadCommand (uint8_t *pbArrayWithTheCommanad)
{
	uint8_t bCommandsCounter = 0;
	uint8_t bMatch = 1;
	uint8_t abCommand_temp[MAX_NUMBER_OF_CHARACTERS_FOR_ALL_COMMANDS];

	if(pbArrayWithTheCommanad)
	{
		vfnSplitString(SPACE_ASCII_CHARACTER, &pbArrayWithTheCommanad[0], &abCommand_temp[0]);

		while(bCommandsCounter < AMOUNT_OF_COMMANDS)
		{
			bMatch = bfnStrCmp(&gabCommandTable[bCommandsCounter][0], &abCommand_temp[0]);

			if(!bMatch)
			{
				break;
			}

			bCommandsCounter++;
		}
	}

	if(bMatch)
	{
		bCommandsCounter = 0xff;
	}

	return bCommandsCounter;
}

void vfnCLI_RefreshPrompt(void)
{
	if(gevent_handdle.bEspecialEventsFlags & IT_IS_A_REACALL_COMMAND)
	{
		gevent_handdle.bEspecialEventsFlags &=  ~IT_IS_A_REACALL_COMMAND;
	}
	else
	{
		vfnUART_TX(&gab_Prompt[0]);
	}
}

uint8_t bfnGetOperator(uint8_t bMaxNumberOfArguments, uint8_t * pbStringOfArguments, uint8_t * pbStringOfOptions)
{
	uint8_t bCharacterOfTheOption;

	uint8_t bPositionOfTheOptionInTheSourceString;

	uint8_t abOptionStringToSearch[4] =
	{
			"-1 "
	};

	if(pbStringOfOptions[0] != 0)
	{
		if(gbOffsetOfTheOpcion < bMaxNumberOfArguments)
		{
			/* We obtain the character of the option */
			vfnSplitStringV2(&pbStringOfOptions[0], &bCharacterOfTheOption, gbOffsetOfTheOpcion * OPTION_OFFSET +1 , ':');

			gbOffsetOfTheOpcion++;

			abOptionStringToSearch[1] = bCharacterOfTheOption;

			/*Now it is neccesary copy the argument in order to allows us used it. */
			/*first we get the position of where do start the argument. */

			bPositionOfTheOptionInTheSourceString = bfnGetPositionOfToken(&pbStringOfArguments[0], &abOptionStringToSearch[0]);

			if(bPositionOfTheOptionInTheSourceString < 254)
			{
				vfnSplitStringV2(&pbStringOfArguments[0], &gabArgumentOfTheCommand[0], bPositionOfTheOptionInTheSourceString, SPACE_ASCII_CHARACTER);
			}
			else
			{
				bCharacterOfTheOption = 0xff;

				gbOffsetOfTheOpcion = 0;
			}

		}
		else
		{
			bCharacterOfTheOption = 0xFF;

			gbOffsetOfTheOpcion = 0;
		}
	}
	else
	{
		bCharacterOfTheOption = 0xFF;
	}

	return bCharacterOfTheOption;
}

#ifdef _V2_
uint8_t bfnGetOperatorV2(uint8_t bMaxNumberOfArguments, uint8_t * pbStringOfArguments, uint8_t * pbStringOfOptions)
{
	uint8_t bCharacterOfTheOption;

	uint8_t bPositionOfTheOptionInTheSourceString;

	uint8_t abOptionStringToSearch[4] =
	{
			"-1 "
	};

	if(pbStringOfOptions[0] == 0)
	{
		bCharacterOfTheOption = 0xFF;

		return bCharacterOfTheOption;
	}

	if(gbOffsetOfTheOpcion >= bMaxNumberOfArguments)
	{
		bCharacterOfTheOption = 0xFF;

		gbOffsetOfTheOpcion = 0;

		return bCharacterOfTheOption;
	}

	/* We obtain the character of the option */
	vfnSplitStringV2(&pbStringOfOptions[0], &bCharacterOfTheOption, gbOffsetOfTheOpcion * OPTION_OFFSET +1 , ':');

	gbOffsetOfTheOpcion++;

	abOptionStringToSearch[1] = bCharacterOfTheOption;

	/*Now it is neccesary copy the argument in order to allows us used it. */
	/*first we get the position of where do start the argument. */

	bPositionOfTheOptionInTheSourceString = bfnGetPositionOfToken(&pbStringOfArguments[0], &abOptionStringToSearch[0]);

	if(bPositionOfTheOptionInTheSourceString)
	{
		vfnSplitStringV2(&pbStringOfArguments[0], &gabArgumentOfTheCommand[0], bPositionOfTheOptionInTheSourceString, SPACE_ASCII_CHARACTER);
	}

	return bCharacterOfTheOption;
}

#endif

uint8_t bfnGetPositionOfToken(uint8_t * pbSourceString, uint8_t * pbToken)
{
	uint8_t bCounterOfSourceString = 0;

	uint8_t bAreThereMatch_flag = 0;

	uint8_t bCounterOfMatches = 0;

	/* the condition below is only to know if the direction of the string is valid.*/
	if (&pbSourceString[0] && &pbToken[0])
	{
		while(pbSourceString[bCounterOfSourceString] != NULL_CHARACTER)
		{
			if(!pbToken[bCounterOfMatches])
			{
				bAreThereMatch_flag = 1;

				break;
			}

			/* We count the matches until the String in the token turns 0, when that occours this means that we founde the position of the token*/
			if(pbSourceString[bCounterOfSourceString] == pbToken[bCounterOfMatches])
			{
				bCounterOfMatches++;
			}
			else
			{
				bCounterOfMatches = 0;
			}

			bCounterOfSourceString++;
		}
		/* there aren't match, return the error value. */
		if(!bAreThereMatch_flag)
		{
			bCounterOfSourceString = 0xfd;
		}
	}
	else
	{
		/* Return error if whatever address is 0.*/
		bCounterOfSourceString = 0xfd;
	}

	/*+1 in order to delete all characters of the token. */
	return bCounterOfSourceString + 1;
}

static void vfnHelpCommand(void)
{
	vfnUART_TX(&gab_msj_Help[0]);
	vfnUART_TX(&gab_msj_Help2[0]);
}

static void vfnChagePrompt(void)
{
	uint8_t bCharacterOfTheOption = 0;

	uint8_t bNumberOfCharacters = 0;

	bCharacterOfTheOption = bfnGetOperator(1, &gab_CLIData[0], &gabChangePromptOption[0]);

	if(bCharacterOfTheOption != gabChangePromptOption[0])
	{
		return;
	}

	bNumberOfCharacters = wfnStrLen(&gabArgumentOfTheCommand[0]);

	if(bNumberOfCharacters > MAX_NUMBER_OF_CHARATERS_FOR_PROMPT)
	{
		return;
	}

	vfnMemCopyV2(&gabArgumentOfTheCommand[0], &gab_Prompt[0], INIT_POSITION_OF_PROMT);
}

/* In construction.*/
static void vfnRawStepsCommand(void)
{
	uint8_t bError_flag = 0;

	uint8_t bCharacterOfTheOption = 0;

	uint16_t wRawNumberOfSteps = 0;

	uint8_t bRawNumberOfMotor = 0;

	while(bCharacterOfTheOption < 0xfe)
	{
		bCharacterOfTheOption = bfnGetOperator(2, &gab_CLIData[0], &gabRawStepOptions[0]);

		switch(bCharacterOfTheOption)
		{
			case 'm':
			{
				bRawNumberOfMotor = (uint8_t) wfnBCDToBinary (&gabArgumentOfTheCommand[0]);
				break;
			}
			case 's':
			{
				wRawNumberOfSteps = wfnBCDToBinary (&gabArgumentOfTheCommand[0]);
				break;
			}
			case 0xfe:
			{
				/* syntaxis error. */
				vfnUART_TX(&gab_MsjCommandDoesntExist[0]);
				bError_flag = 1;
				break;
			}
			default:
			{

			}
		}
	}

	if(!bError_flag)
	{
		vfnMotor_rawSteps(bRawNumberOfMotor, wRawNumberOfSteps);

		vfnUART_TX(&gabRawStepMSJ[0]);
	}
}

static void vfnChangeFrequencyCommand(void)
{
	uint8_t bCharacterOfTheOption = 0;

	uint16_t wRawFrequency;

	while(bCharacterOfTheOption != 0xff)
	{
		bCharacterOfTheOption = bfnGetOperator(1, &gab_CLIData[0], &gabRawFrequencyOptions[0]);

		if(bCharacterOfTheOption == gabRawFrequencyOptions[0])
		{
			wRawFrequency =  wfnBCDToBinary (&gabArgumentOfTheCommand[0]);
		}
	}

	vfnMotor_rawFrequency(wRawFrequency);
}

static inline void vfnFrequecyMSJ(void)
{
	vfnUART_TX(&gabFrequency_MSJ[0]);
}

static inline void vfnPORTsMSJ(void)
{
	uint8_t bCounterOfMotors = 0;

	vfnUART_TX(&gabPortShow_MSJ[0]);

	/*"ptx10\ttpm0\tch0\t\t"*/

	while(bCounterOfMotors < NUMBER_OF_MOTORS)
	{
		vfnGetInformationPort(&gabPortInformation_MSJ[0], bCounterOfMotors);

		gabPortInformation_MSJ[5] = '\t';
		//gabPortInformation_MSJ[7] = '\t';
		gabPortInformation_MSJ[10] = '\t';
		//gabPortInformation_MSJ[13] = '\t';
		//gabPortInformation_MSJ[1] = '\t';
		//gabPortInformation_MSJ[18] = '\t';
		gabPortInformation_MSJ[14] = '\n';
		gabPortInformation_MSJ[15] = '\r';

		vfnUART_TX(&gabPortInformation_MSJ[0]);
		vfnUART_TX(&gabPortInformation_MSJ[0]);

		bCounterOfMotors++;

	}
}

static inline void vfnCommandsMSJ(void)
{
	uint8_t bConunterCommands =0;

	while (bConunterCommands < AMOUNT_OF_COMMANDS)
	{
		vfnPrintln();

		vfnUART_TX(&gabCommandTable[bConunterCommands][0]);

		bConunterCommands++;
	}
}

static inline void vfnHelpShowMSJ(void)
{
	uint8_t bConunterShowOptions = 0;

	vfnUART_TX(&gabWarningOptionCommandMSJ[0]);

	while (bConunterShowOptions < AMOUNT_OF_SHOW_OPTIONS)
	{
		vfnPrintln();

		vfnUART_TX(&gabShowTable[bConunterShowOptions][0]);

		bConunterShowOptions++;
	}
}

static inline void vfnPrintln(void)
{
	vfnUART_TX(&gabEnterASCII[0]);
}

void vfnShowCommand(void)
{
	uint16_t wXValue;

	//uint8_t abStringOfNumbers[5];

	uint8_t abCommand_temp[15];

	uint8_t bMatch = 1;

	uint8_t bShowCounter = 0;

	vfnSplitString(NULL_CHARACTER, &gab_CLIData[0], &abCommand_temp[0]);

	while(bShowCounter < AMOUNT_OF_SHOW_OPTIONS)
	{
		bMatch = bfnStrCmp(&gabShowTable[bShowCounter][0], &abCommand_temp[0]);

		if(!bMatch)
		{
			break;
		}

		bShowCounter++;
	}

	switch(bShowCounter)
	{
		case FREQUENCY_SHOW:
		{
			wXValue = wfnMotor_getModuleConf();

			vfnBinaryToBCD16(wXValue,&gabFrequency_MSJ[16]);

			wXValue = wfnMotor_getFrequency();

			vfnBinaryToBCD16(wXValue,&gabFrequency_MSJ[34]);

			vfnFrequecyMSJ();

			break;
		}
		case PORTS_SHOW:
		{
			vfnPORTsMSJ();
			break;
		}
		case COMMANDS_SHOW:
		{
			vfnCommandsMSJ();
			break;
		}
		default:
		{
			vfnHelpShowMSJ();
			/* do something*/
		}
	}
}

static void vfnChangeDirectionCommand (void)
{
	uint8_t bError_flag = 0;

	uint8_t bCharacterOfTheOption = 0;

	uint8_t bDirection= 0;

	uint8_t bRawNumberOfMotor = 0;

	while(bCharacterOfTheOption < 0xfe)
	{
		bCharacterOfTheOption = bfnGetOperator(2, &gab_CLIData[0], &gabChageDirectionOptions[0]);

		switch(bCharacterOfTheOption)
		{
			case 'm':
			{
				bRawNumberOfMotor = (uint8_t) wfnBCDToBinary (&gabArgumentOfTheCommand[0]);
				break;
			}
			case 'd':
			{
				bDirection = (uint8_t) wfnBCDToBinary (&gabArgumentOfTheCommand[0]);
				break;
			}
			case 0xfe:
			{
				/* syntaxis error. */
				bError_flag = 1;
				break;
			}
			default:
			{

			}
		}
	}

	if(!bError_flag)
	{
		vfnMotor_rawChangeDirection(bRawNumberOfMotor, bDirection);
	}
}

static void inline vfnEncoderLoopHelloMSJ(void)
{
	vfnUART_TX(&gabEncoderLoopHelloMSJ[0]);
}

static void vfnEncoderCommand(void)
{
	uint8_t bNumberOfMotor;

	uint8_t bError_flag = 1;

	uint8_t bCharacterOfTheOption = 0;

	while(bCharacterOfTheOption != 0xff)
	{
		bCharacterOfTheOption = bfnGetOperator(1, &gab_CLIData[0], &gabEncoderOptions[0]);

		if(bCharacterOfTheOption == gabEncoderOptions[0])
		{
			bNumberOfMotor = (uint8_t) wfnBCDToBinary (&gabArgumentOfTheCommand[0]);

			bError_flag = 0;
		}
	}

	if(bError_flag == 0)
	{
		gconf_Handdle.bEvent_flag = ENCODER_STREM_ESPCIAL_EVENT;

		gevent_handdle.bMotor = bNumberOfMotor;

		vfnEncoderLoopHelloMSJ();
	}
}

void vfnEncoderLoop(void)
{
	uint16_t wActualPosition;

	wActualPosition = wfnMotor_getEncoderPosition(gevent_handdle.bMotor);

	if(wActualPosition != gevent_handdle.wEncoderTempBuffer)
	{
		vfnBinaryToBCD16(wActualPosition, &gabEncoderLoopEventMSJ[13]);

		vfnUART_TX(&gabEncoderLoopEventMSJ[0]);
	}

	gevent_handdle.wEncoderTempBuffer = wActualPosition;
}

static inline void vfnEncoderPoolMSJ(void)
{
	vfnUART_TX(&gabEncoderPoolMSJ[0]);
}

static void vfnSetHomeCommand (void)
{
	uint8_t bNumberOfMotor;

	uint8_t bError_flag = 1;

	uint8_t bCharacterOfTheOption = 0;

	while(bCharacterOfTheOption != 0xff)
	{
		bCharacterOfTheOption = bfnGetOperator(1, &gab_CLIData[0], &gabEncoderOptions[0]);

		if(bCharacterOfTheOption == gabEncoderOptions[0])
		{
			bNumberOfMotor = (uint8_t) wfnBCDToBinary (&gabArgumentOfTheCommand[0]);

			bError_flag = 0;
		}
	}

	if(bError_flag == 0)
	{
		gevent_handdle.bMotor = bNumberOfMotor;

		gconf_Handdle.bEvent_flag = ENCONDER_LOOP_EVENT;

		vfnEncoderPoolMSJ();
	}

}

void vfnCLI_SetZeroEncoder(void)
{
	vfnMotor_setZeroEncoder(gevent_handdle.bMotor);

	gconf_Handdle.bEvent_flag = 0;

	gconf_Handdle.bkeyEvent = 0;

	gb_CouterOfCharacters = 0;
}

static void vfnMachineControlCommand(void)
{
	gconf_Handdle.bEvent_flag = MACHINE_CONTROL_EVENT;

	vfnResetMotorStatusFlag();

	vfnSwTimers_StartTimer(gs_handshakeTimer.bGetTimer, 10);
}

static inline uint8_t vfngetIDMotor(void)
{

	return gab_CLIData[1] - OFFSET_ASCCI_NUMBERS;
}

static inline uint16_t wfngetSteps(void)
{
	return wfnBCD5ToBinary16(&gab_CLIData[2]);
}

static inline uint8_t bfngetDirection(void)
{
	return gab_CLIData[7] - OFFSET_ASCCI_NUMBERS;
}

static inline uint16_t wfnGetFrequency(void)
{
	return wfnBCD5ToBinary16(&gab_CLIData[1]);
}

static inline void vfnProccesingMachineFrame(void)
{
	uint8_t bHeader;

	uint8_t bNMotorNumber;

	uint8_t bDirection;

	uint16_t wSteps;

	uint16_t wFrequency;

	if(gconf_Handdle.bkeyEvent == ENTER_KEY_EVENT)
	{
		bHeader = gab_CLIData[0];

		switch(bHeader)
		{
			case 'm':
			{
				bNMotorNumber = vfngetIDMotor();

				wSteps = wfngetSteps();

				bDirection = bfngetDirection();

				vfnMotor_rawChangeDirection(bNMotorNumber, bDirection);

				vfnMotor_rawSteps(bNMotorNumber, wSteps);

				break;
			}
			case 'f':
			{
				wFrequency = wfnGetFrequency();

				vfnMotor_rawFrequency(wFrequency);

				break;
			}
			default:
			{

			}

		}

		gb_CouterOfCharacters = 0;

		gconf_Handdle.bkeyEvent = 0;
	}
}

static inline void vfnSendTheIDOfTheMotorThatFinish(uint8_t bMotorNumber)
{
	/*
	uint8_t baBufferToSend[2] =
	{
			bMotorNumber+ OFFSET_ASCCI_NUMBERS,
			0
	};
	*/
	/* El dato a enviar no puede estar declarado en el ambito de una función, porque
	 * el uart almacena la dirección, y las variables locales se almacenan en el stack
	 * por lo que el uart se queda con una dirección apuntando al stack, donde
	 * no tengo control de si se mantiene los valores o no.*/

	gabMachineControlFeedBack[0] = bMotorNumber + OFFSET_ASCCI_NUMBERS;

	vfnUART_TX(&gabMachineControlFeedBack[0]);
}

static inline void vfnCheckWichMotorAlreadyFinish(void)
{
	uint8_t bNumberOfTheMotorThatFinish = 0xff;

	bNumberOfTheMotorThatFinish = vfnMotor_checkWichMotorAlredyFinish();

	if(bNumberOfTheMotorThatFinish != 0xff)
	{
		vfnSendTheIDOfTheMotorThatFinish(bNumberOfTheMotorThatFinish);

	}
}

void vfnCLI_MachineControlTasks (void)
{
	vfnProccesingMachineFrame();

	vfnCheckWichMotorAlreadyFinish();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Preprocces function section
///////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t bfnThereAreOverflowAndCounterIsZero(void)
{
	return (gevent_handdle.bEspecialEventsFlags & OVERFLOW_TEMP_BUFFER_MASK) & gevent_handdle.bCounterOfTempBuffer;
}

static void vfnValidateUP_Key_event(void)
{
	if(bfnThereAreOverflowAndCounterIsZero())
	{
		gevent_handdle.bCursorPointerOfTEmpBuffer = AMOUNT_OF_TEMP_COMMANDS;
	}
	else
	{
		if(gevent_handdle.bCounterOfTempBuffer)
		{
			gevent_handdle.bCursorPointerOfTEmpBuffer--;
		}
		else
		{
			gevent_handdle.bCursorPointerOfTEmpBuffer = gevent_handdle.bCursorPointerOfTEmpBuffer;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Avoid Problems function section
///////////////////////////////////////////////////////////////////////////////////////////////////

static inline void vfnSendHelloMsj (void)
{
	vfnUART_TX(&gabHello3WHMSJ[0]);
}

static inline void vfnSendACKMsj (void)
{
	vfnUART_TX(&gabACK3WHMSJ[0]);
}

uint8_t bfnCLI_3WayHandshake (void)
{
	uint8_t bConnection = 0;

	vfnSwTimers_Task();

	gs_handshakeTimer.bStatus = bfnSwTimers_GetStatus(gs_handshakeTimer.bGetTimer);

	if(gs_handshakeTimer.bStatus)
	{
		vfnSendHelloMsj();

		vfnSwTimers_StartTimer(gs_handshakeTimer.bGetTimer, HANDSHAKE_TIMEOUT_MS);
	}
	else
	{
		if(gab_CLIData[0] == REPLY_OF_MACHINE)
		{
			gb_CouterOfCharacters = 0;

			vfnSendACKMsj();

			bConnection = 1;
		}
	}

	return bConnection;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CALLBACKS
///////////////////////////////////////////////////////////////////////////////////////////////////

static void vfnCallBack_UART (uint8_t bByteOfTheUART)
{
	gab_CLIData[gb_CouterOfCharacters] = bByteOfTheUART;

	if(bByteOfTheUART == ENTER_ASCII)
	{
		gconf_Handdle.bkeyEvent = ENTER_KEY_EVENT;
	}
	else if(bByteOfTheUART == DEL_ASCII)
	{
		if(gb_CouterOfCharacters != 0)
		{
			gb_CouterOfCharacters--;
		}
	}
	else if(bByteOfTheUART == UP_KEY_ASCII)
	{
		gconf_Handdle.bkeyEvent = UP_KEY_EVENT;
	}
	else if(bByteOfTheUART == CTRL_C_ASCII)
	{
		gconf_Handdle.bkeyEvent = CTRL_KEY_BREAK_EVENT;
	}
	else
	{
		gb_CouterOfCharacters++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
