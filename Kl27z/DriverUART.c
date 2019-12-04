/*
 * DriverUART.c
 *
 *  Created on: Mar 20, 2018
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "DriverUart.h"

#include "MKL27Z644.h"

#include "GeneralFunctions.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#define TRUE            (1)

#define FALSE           (0)

#define UART_RX_PIN     (1)

#define UART_TX_PIN             (2)

#define PCR_UART_ALT            (2)

#define IRQ_UART                (12)

#define CORE_CLOCK              (48000000)

#define BAUD_RATE               (115200)

#define OVER_SAMPLIN_RATIO      (16)            /*default-> 15 Without count 0*/

#define UART_BAUD_RATE_DIVISOR  ((CORE_CLOCK/BAUD_RATE)/OVER_SAMPLIN_RATIO )

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t     *pbStringToSend;
    uint8_t     bCounterOfDataTransmitted;
    UARTAplicationCallback_t UARTCallBack;
} tx_data_handdle;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static void vfnInintUARTPin(void);

static void vfnUARTTXInterruptEnable(uint8_t bEnableInterrupts);
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
static tx_data_handdle TxDataHanddle;

static uint8_t gbDefaultDirectionForTheBufferToSend = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static void vfnInintUARTPin(void)
{
    /** inits Clocks **/
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SCGC5 |= SIM_SCGC5_LPUART0_MASK;
    SIM->SOPT2 |= SIM_SOPT2_LPUART0SRC(1);

    /** Init RX and TX port **/
    PORTA->PCR[UART_RX_PIN] |= PORT_PCR_MUX(PCR_UART_ALT);
    PORTA->PCR[UART_TX_PIN] |= PORT_PCR_MUX(PCR_UART_ALT);

    /** active NVIC **/
    NVIC->ISER[0] |= (1 << IRQ_UART);
}

void vfnUART_Init (UARTAplicationCallback_t UARTRxCallBack)
{
    vfnInintUARTPin();

    TxDataHanddle.UARTCallBack = UARTRxCallBack;

    /* BAUD.*/
    /* Set baud rate and number of stop bits */
    LPUART0->BAUD &= ~LPUART_BAUD_SBR_MASK;
    LPUART0->BAUD |= UART_BAUD_RATE_DIVISOR;
    LPUART0->BAUD |= LPUART_BAUD_SBNS(0);
    //LPUART0->BAUD |= LPUART_BAUD_OSR(OVER_SAMPLIN_RATIO);

    /* CTRL. */
    /* set the important configurations, like interrupts and the format 8N1 */
    /* ENABLE interrupts of Rx (the interrupts of TX are in the function Tx).*/

    LPUART0->CTRL |= LPUART_CTRL_RIE(1);        /* enable Recive interrupt */

    /* mode of send data */

    LPUART0->CTRL |= LPUART_CTRL_IDLECFG(0);    /* NUMBER OF idle bits */
    LPUART0->CTRL |= LPUART_CTRL_M(0);          /* set data with 8 bits for character. */
    LPUART0->CTRL |= LPUART_CTRL_PE(0);         /* disable parity bit */

    LPUART0->CTRL |= LPUART_CTRL_RE_MASK;
}

void vfnUART_TX (uint8_t * pbBufferToSend)
{
    /* if there aren't a transmission in progress, initialize a new transmission, otherwise */
    /* wait for the previous transmission to end.*/
    while(!(LPUART0->STAT & LPUART_STAT_TC_MASK))
    {
        /* wait */
    }

    /* set the new pointer, the new buffer size, and reset the offset*/
    TxDataHanddle.pbStringToSend = pbBufferToSend;
    TxDataHanddle.bCounterOfDataTransmitted = 0;

    vfnUARTTXInterruptEnable(TRUE);
}

static void vfnUARTTXInterruptEnable(uint8_t bEnableInterrupts)
{
    if(bEnableInterrupts)
    {
        LPUART0->CTRL |= LPUART_CTRL_TE_MASK;

        LPUART0->CTRL |= LPUART_CTRL_TIE_MASK;        /* enable transmit interrupt */

        LPUART0->CTRL |= LPUART_CTRL_TCIE_MASK;       /* enable transmit complete interrupt */
    }
    else
    {
        LPUART0->CTRL &= ~LPUART_CTRL_TE_MASK;

        LPUART0->CTRL &= ~LPUART_CTRL_TIE_MASK;    /* disable transmit interrupt */

        LPUART0->CTRL &= ~LPUART_CTRL_TCIE_MASK;   /* disable transmit complete interrupt */

        //TxDataHanddle.pbStringToSend = &gbDefaultDirectionForTheBufferToSend;


        //TxDataHanddle.bCounterOfDataTransmitted = 0;
    }
}


void LPUART0_IRQHandler(void)
{
    if ((LPUART0->STAT & LPUART_STAT_TDRE_MASK) && (LPUART0->CTRL & LPUART_CTRL_TE_MASK))
    {
        if(TxDataHanddle.pbStringToSend[TxDataHanddle.bCounterOfDataTransmitted])
        {
            LPUART0->DATA |= TxDataHanddle.pbStringToSend[TxDataHanddle.bCounterOfDataTransmitted];

            TxDataHanddle.bCounterOfDataTransmitted++;
        }
    }

    if((LPUART0->STAT & LPUART_STAT_TC_MASK) && (LPUART0->CTRL & LPUART_CTRL_TE_MASK))
    {
        vfnUARTTXInterruptEnable(FALSE);
    }

    if(LPUART0->STAT & LPUART_STAT_RDRF_MASK)
    {
        TxDataHanddle.UARTCallBack(LPUART0->DATA);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
