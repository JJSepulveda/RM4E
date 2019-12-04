/*
 * GeneralFunctions.c
 *
 *  Created on: Mar 8, 2018
 *      Author: JJ
 */
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "GeneralFunctions.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#define NUMBER_OF_BITS_FOR_EACH_BCD (8)

#define DECIMAL_BASE                (10)

#define NULL_CHARACTER              (0)

#define MAX_NUMBER_OF_BCD           (7)

#define FIRST_NUMBER_IN_ASCII_CODE  (0x30)

#define LAST_NUMBER_IN_ASCII_CODE   (0x39)

#define DECENAS_DE_MILLAR			(5)

#define AMOUNT_OF_NUMBERS_TO_BCD16	(5)

#define OFFSET_OF_NUMBERS			(48)
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

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t dwfnBinaryToBCD (uint32_t wBinaryNumber, uint32_t dwMaxNumberOfBCDDigits)
{
    uint32_t dwBCDNumber = 0;
    uint8_t bCounterOfDigit = 0;

    /*Se guardan los numeros al revez*/
    while (dwMaxNumberOfBCDDigits > bCounterOfDigit)
    {
        dwBCDNumber = dwBCDNumber << NUMBER_OF_BITS_FOR_EACH_BCD;
        dwBCDNumber |= (wBinaryNumber % DECIMAL_BASE);
        wBinaryNumber /= 10;
        bCounterOfDigit++;
    }

    return dwBCDNumber;
}

void vfnDelayMiliSeconds (uint32_t dwTiempoMiliSegundos)
{
    uint32_t dwTiemeToSetFlag = 20000;
    uint32_t dwFrecuencyCore = 48000000;
    uint32_t dwFrecuencyToCount = dwFrecuencyCore / dwTiemeToSetFlag;

    uint32_t dwContadorDelay2 = 0;
    //uint8_t bStatusFlag = 0;
    uint32_t dwContadorMiliSegundos = 0;

    while (dwContadorMiliSegundos < dwTiempoMiliSegundos)
    {
        while(dwContadorDelay2 < dwFrecuencyToCount)
        {
                dwContadorDelay2++;
        }
        dwContadorMiliSegundos++;
        dwContadorDelay2= 0;
    }
}

uint16_t wfnStrLen (uint8_t *bpString)
{
    uint16_t wContador = 0;

    if(bpString)
    {
        while (bpString[wContador])
        {
            wContador++;
        }
    }

    return wContador;
}

uint32_t dwfnPromedio (uint16_t *pdwBufferFuente, uint32_t dwCantidadDatos)
{
    if((pdwBufferFuente)&&(dwCantidadDatos))
    {
        uint32_t dwContador = 0;
        uint32_t dwAcomulador = 0;

        while (dwCantidadDatos>dwContador)
        {
            dwAcomulador += pdwBufferFuente[dwContador];
            dwContador++;
        }

        return (dwAcomulador/dwCantidadDatos);
    }

    return 0;

}

void vfnMemCopy (uint8_t *bpBufferFuente, uint8_t *bpBufferDestino, uint32_t dwCantidad)
{
    uint32_t dwContador = 0;

    if((bpBufferFuente && bpBufferDestino && dwCantidad)){

        while(dwCantidad > dwContador){
            bpBufferDestino[dwContador] = bpBufferFuente[dwContador] ;
            dwContador++;
        }
    }
}

void vfnMemCopyV2 (uint8_t *bpBufferFuente, uint8_t *bpBufferDestino, uint8_t bInitPosition)
{
    uint32_t dwContador = bInitPosition;

    if(bpBufferFuente && bpBufferDestino)
    {
        while(bpBufferFuente[dwContador - bInitPosition])
        {
            bpBufferDestino[dwContador] = bpBufferFuente[dwContador-bInitPosition];
            dwContador++;
        }
        bpBufferDestino[dwContador] = 0;
    }

}

/*
 * vfnSplitString
 * This function only copy in a new buffer the data of the start count until the token.
 */
void vfnSplitStringV2(uint8_t * pbSourceString, uint8_t * pbTargetString, uint8_t bStartCount, uint8_t btoken)
{
    uint8_t bCount = bStartCount - 1;

    uint8_t bCountOfDataToCopy = 0;

    uint8_t bAmountOfCharactersBeforeToToken;

    /* The condition below allows us to avoid  problems. */
    if(btoken != 0)
    {
        /* Count the number of exist characters that there are between of the StarCount and the token.*/
        while(pbSourceString[bCount] != btoken && pbSourceString[bCount] != 0)
        {
            bCount++;
        }

        /* the amount of that will copy */
        bAmountOfCharactersBeforeToToken = bCount - (bStartCount - 1);

        /* Copy to the String source to target String the amount of data. */
        while(bCountOfDataToCopy < bAmountOfCharactersBeforeToToken)
        {
            pbTargetString[bCountOfDataToCopy] = pbSourceString[bStartCount - 1 + bCountOfDataToCopy];

            bCountOfDataToCopy++;
        }

        /* Add the null character to the target string. */
        pbTargetString[bCountOfDataToCopy] = 0;
    }
}

/*
 * vfnSplitString
 * This function copy the data to the init of the string until the token.
 * This function split the source string of the token until the end of the string.
 */

void vfnSplitString (uint8_t bToken,uint8_t *bpSourceBuffer, uint8_t *bpTargetBuffer)
{
    uint16_t wCounterOfCharacteres = 0;
    uint16_t wSizeOfSourceBuffer   = 0;
    uint16_t wNewBufferSourceSize  = 0;

    if(bpSourceBuffer && bpTargetBuffer)
    {
        /* First count the number of characters before of the token. */
        while(bpSourceBuffer[wCounterOfCharacteres] != NULL_CHARACTER && bpSourceBuffer[wCounterOfCharacteres] != bToken)
        {
            wCounterOfCharacteres++;
        }

        if(wCounterOfCharacteres == 0)
        {
            bpTargetBuffer[0] = 0;
        }
        else
        {
            vfnMemCopy(bpSourceBuffer, bpTargetBuffer,wCounterOfCharacteres);
            bpTargetBuffer[wCounterOfCharacteres] = 0;
        }
        /* the source buffer change too, it is removed the string that it was put in the target buffer.*/
        wSizeOfSourceBuffer = wfnStrLen(bpSourceBuffer);
        wNewBufferSourceSize = wSizeOfSourceBuffer - wCounterOfCharacteres;

        if(wNewBufferSourceSize == 0)
        {
            bpSourceBuffer[0] = 0;
        }
        else
        {
            /* It needs to plus one, otherwise, the new source string It will starts in the delimiter place and
             * when I will need to execute this function it will not do what I need it to do
             */
            vfnMemCopy(&bpSourceBuffer[wCounterOfCharacteres+1], bpSourceBuffer,wNewBufferSourceSize );
            bpSourceBuffer[wNewBufferSourceSize] = 0;
        }
    }
}

uint8_t bfnStrCmp (uint8_t *bpCadena1, uint8_t *bpCadena2){
    uint32_t dwContador = 0;
    uint8_t bSwFlagToMatch = 1; /* one means that the strings are different */

    /* Retorna 0 si las cadenas son iguales, 1 si son diferentes*/
    if(bpCadena1 && bpCadena2)
    {
        while ((bpCadena1[dwContador] == bpCadena2[dwContador]) && bSwFlagToMatch)
        {
            if(bpCadena1[dwContador] == 0)
            {
                bSwFlagToMatch = 0;
            }
            else
            {
                dwContador++;
            }
        }
    }
    return bSwFlagToMatch;
}

uint16_t wfnSearchTheStringInArray (uint8_t * pbStringToSearch, uint8_t * pbArray, uint16_t wMaxNumberOfStringsInArray,uint16_t wMaxNumberOfCharactersEachString)
{
    /* if there is a match, return the position of the string in the array, otherwise, return the max number. */
    uint16_t wCounterOfPosition = 0;
    uint8_t bMatchString = 1;           /* one it means that the strings are different */

    if(pbStringToSearch && pbArray && wMaxNumberOfStringsInArray)
    {
        while((wCounterOfPosition < wMaxNumberOfStringsInArray) && (bMatchString))
        {
            bMatchString = bfnStrCmp(pbStringToSearch, &pbArray[wCounterOfPosition*wMaxNumberOfCharactersEachString]);
            wCounterOfPosition++;
        }

        if(bMatchString == 0)
        {
            /* it needs to subtract one to convert in an index to array */
            wCounterOfPosition--;
        }
    }
    else
    {
        wCounterOfPosition = wMaxNumberOfStringsInArray;
    }

    return wCounterOfPosition;
}

uint32_t dwfnPower(uint8_t bNumberToRaise, uint8_t bPower)
{
    /* set one because any number raised to power one is the same number */
    uint8_t bCounterUntilTheMaxPower = 1;
    uint8_t bBaseNumber = bNumberToRaise;
    uint32_t dwNumberRaise = bNumberToRaise;

    if(bPower)
    {
        while(bCounterUntilTheMaxPower < bPower)
        {
            dwNumberRaise *= bBaseNumber;
            bCounterUntilTheMaxPower++;
        }
    }
    else
    {
        dwNumberRaise = 1;
    }

    return dwNumberRaise;
}

uint16_t wfnBCDToBinary (uint8_t * pbNumberInBCD)
{
    /* As the function returns 4 bytes, this means that the mayor number that It can return is 65,536 */
    uint8_t bNumberOfBCD                        = 0;
    uint8_t bFlagToKnowIfCharacterIsValidNumber = 1; /* one means that the character is valid number */
    uint8_t bIndexOfTheNumber                   = 0;
    uint8_t bBCDInBinary                        = 0;
    uint16_t wAmountOfBCDInBinary               = 0;

    if(pbNumberInBCD)
    {
        bNumberOfBCD = wfnStrLen(pbNumberInBCD);

        if(bNumberOfBCD < MAX_NUMBER_OF_BCD)
        {
            while ( (bIndexOfTheNumber < bNumberOfBCD) && (bFlagToKnowIfCharacterIsValidNumber))
            {
                /* first, it is needed to check if the number is in the range. */
                if((pbNumberInBCD[bIndexOfTheNumber] >= FIRST_NUMBER_IN_ASCII_CODE) && (pbNumberInBCD[bIndexOfTheNumber] <= LAST_NUMBER_IN_ASCII_CODE))
                {
                    bBCDInBinary = pbNumberInBCD[bIndexOfTheNumber] - FIRST_NUMBER_IN_ASCII_CODE;
                    /* y = 10^((max_number_of_BCD-1) - x) ; this function is for obtain units, tens, hundreds and thousands*/
                    wAmountOfBCDInBinary += (bBCDInBinary * dwfnPower(10, (bNumberOfBCD - 1) - bIndexOfTheNumber));
                    bIndexOfTheNumber++;
                }
                else
                {
                    bFlagToKnowIfCharacterIsValidNumber = 0;
                    wAmountOfBCDInBinary = 0;
                }

            }
        }

    }

    return wAmountOfBCDInBinary;
}

uint16_t wfnBCD5ToBinary16 (uint8_t * pbNumberInBCD)
{
	uint8_t bBinaryNumber;

	uint16_t wBinaryNumber = 0;

	uint16_t wUnits = 1;

	uint8_t bCounter = AMOUNT_OF_NUMBERS_TO_BCD16;

	if(pbNumberInBCD)
	{
		while(bCounter)
		{
			bBinaryNumber = pbNumberInBCD[bCounter-1] - OFFSET_OF_NUMBERS;

			wBinaryNumber += bBinaryNumber * wUnits;

			wUnits *= 10;

			bCounter--;
		}
	}

	return wBinaryNumber;
}

void vfnBCDToBinary16 (uint16_t wValueToConvert, uint8_t * pabTargetToSave)
{
	uint16_t wRawValue;

	uint8_t bCounter = 0;

	uint16_t wUnits = 10000;

	if(pabTargetToSave)
	{
		while(bCounter < DECENAS_DE_MILLAR)
		{
			wRawValue = wValueToConvert % wUnits;

			wValueToConvert -= wRawValue*wUnits;

			wUnits /= 10;

			if(wRawValue == 0)
			{
				wRawValue = 32;
			}
			else
			{
				wRawValue += FIRST_NUMBER_IN_ASCII_CODE;
			}

			pabTargetToSave[bCounter] = wRawValue;

			bCounter++;
		}
	}
}

void vfnBinaryToBCD16 (uint16_t wNumerBinario, uint8_t *pbArregloDeDestino)
{
    uint8_t bContadorDeDigitos = DECENAS_DE_MILLAR;

    uint16_t wManipuladorDelNumeroBinario = wNumerBinario;

    if(pbArregloDeDestino)
    {
		/*Se guardan los numeros al revez*/
		while (bContadorDeDigitos)
		{
			pbArregloDeDestino[bContadorDeDigitos-1] = (wManipuladorDelNumeroBinario % 10) + FIRST_NUMBER_IN_ASCII_CODE;

			wManipuladorDelNumeroBinario /= 10;

			bContadorDeDigitos--;
		}
    }
}

void vfnMemSet16 (uint16_t * wpaTarget, uint8_t bAmount, uint8_t bValue)
{
	uint8_t bCouter = 0;

	if(wpaTarget)
	{
		while(bAmount > bCouter)
		{
			wpaTarget[bCouter] = bValue;

			bCouter ++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
