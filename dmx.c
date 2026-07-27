#include "dmx.h"
#include "stm32f4xx.h"
#include <string.h>

/*
 * Complete DMX packet:
 *
 * dmxFrame[0]   = start code
 * dmxFrame[1]   = channel 1
 * dmxFrame[2]   = channel 2
 * ...
 * dmxFrame[512] = channel 512
 */
DMX_SetChannel(channel, value);

//-----------------------------------------------------------------------------------------------------------------
//DELAY

static void DMX_DelayInit(void)
{

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //Enable access to the Cortex-M4 debug and trace hardware.

    DWT->CYCCNT = 0U; //reset cycle counter

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //Start the cycle counter.
}

static void DMX_DelayUs(uint32_t microseconds)
{
    uint32_t startCycles;
    uint32_t requiredCycles;

    /*
     * At 16 MHz:
     *
     * SystemCoreClock / 1,000,000 = 16 cycles per microsecond.
     */
    requiredCycles =
        microseconds * (SystemCoreClock / 1000000U); //Wait until the requested number of CPU cycles has elapsed.

    startCycles = DWT->CYCCNT;

    while ((DWT->CYCCNT - startCycles) < requiredCycles)
    {
    }
}
//--------------------------------------------------------------------------------------------------------------

void DMX_Init(void)
{
	memset(dmxFrame, 0, sizeof(dmxFrame));

	    dmxFrame[0] = 0x00U;

	    DMX_DelayInit();
    /*
     * Clear all 513 bytes.
     *
     * This makes the start code and every channel initially equal to zero.
     */
    memset(dmxFrame, 0, sizeof(dmxFrame));

    /*
     * Start code 0x00 means standard DMX lighting data.
     */
    dmxFrame[0] = 0x00U;
}

void DMX_SetChannel(uint16_t channel, uint8_t value)
{
    /*
     * Valid DMX channel numbers are 1 through 512.
     *
     * Channel zero is invalid because array index zero contains
     * the DMX start code.
     */
    if ((channel >= 1U) && (channel <= DMX_CHANNEL_COUNT))
    {
        dmxFrame[channel] = value;
    }
}

uint8_t DMX_GetChannel(uint16_t channel)
{
    if ((channel >= 1U) && (channel <= DMX_CHANNEL_COUNT))
    {
        return dmxFrame[channel];
    }

    /*
     * Return zero when an invalid channel is requested.
     */
    return 0U;
}

uint8_t *DMX_GetFrame(void)
{
    return dmxFrame;
}

static void DMX_TX_PinToGPIO(void)
{
    /*
     * PA9 MODER bits are bits 19:18.
     */

    GPIOA->MODER &= ~(3U << (9U * 2U)); //Clear bits
    GPIOA->MODER |=  (1U << (9U * 2U)); //output mode
}

static void DMX_TX_PinToUSART(void)
{
    /*
     * PA9 to alternate-function mode.
     */

    GPIOA->MODER &= ~(3U << (9U * 2U)); //clear bits
    GPIOA->MODER |=  (2U << (9U * 2U)); //AF Mode in GPIO

    /*
     * PA9 uses AFRH bits 7:4.
     * AF7 selects USART1_TX.
     */

    GPIOA->AFR[1] &= ~(0xFU << 4U); //Clear bits
    GPIOA->AFR[1] |=  (7U   << 4U); //AF Mode to use USART1 peripheral
}

static void DMX_TX_Low(void)
{
    /*
     * BSRR upper 16 bits reset output pins.
     * PA9 reset bit is 9 + 16 = 25.
     */

    GPIOA->BSRR = (1U << (9U + 16U));
}

static void DMX_TX_High(void)
{
    /*
     * BSRR lower 16 bits set output pins.
     * PA9 set bit is bit 9.
     */

    GPIOA->BSRR = (1U << 9U);
}

static void USART1_WriteByte(uint8_t byte)
{
    while ((USART1->SR & USART_SR_TXE) == 0U) //when TXE register empty, Wait until USART1 is ready to accept another byte.
    {
    }

    USART1->DR = byte; //Write the byte into the lower 8 bits of the data register.
}

void DMX_SendFrame(void)
{
	uint16_t index;
	DMX_TX_PinToGPIO();

	DMX_TX_Low();
	DMX_DelayUs(120U);

	/* new packet begins */

	DMX_TX_High();
	DMX_DelayUs(12U);

	DMX_TX_PinToUSART();

	for (index = 0U; index < DMX_FRAME_SIZE; index++)
	    {
	        USART1_WriteByte(dmxFrame[index]);
	    }

	    while ((USART1->SR & USART_SR_TC) == 0U)
	    {
	    }
}
