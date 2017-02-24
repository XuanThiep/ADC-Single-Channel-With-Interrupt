#include "driverlib.h"
#include "mymsp430.h"


volatile uint16_t adc_value=0;
volatile float	  voltage=0;
void Adc_Init(void);

void main( void )
{
	/* Stop watchdog timer */
	WDT_A_hold(WDT_A_BASE);

	Clk_Using_DCO_Init(16000,8000,SMCLK_CLOCK_DIVIDER_2);

	Adc_Init();

	/* Global Interrupt Enable */
	__enable_interrupt();

	/* Start conversion with repeated mode */
	ADC12_A_startConversion(ADC12_A_BASE,
							ADC12_A_MEMORY_0,
							ADC12_A_REPEATED_SINGLECHANNEL);
	while(1)
	{

	}
}


void Adc_Init(void)
{

	/* Configure P6.4/A4 as input peripheral for ADC input */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,GPIO_PIN4);

	/*
	 * Base address of ADC12_A_A Module
	 * Use internal ADC12_A_A bit as sample/hold signal to start conversion
	 * USE MODOSC 5MHZ Digital Oscillator as clock source
	 * Use default clock divider of 1
	 */
	ADC12_A_init(ADC12_A_BASE,
			ADC12_A_SAMPLEHOLDSOURCE_SC,
			ADC12_A_CLOCKSOURCE_ADC12OSC,
			ADC12_A_CLOCKDIVIDER_1);
	/* Enable ADC Module */
	ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base address of ADC12_A_A Module
     * For memory buffers 0-7 sample/hold for 64 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Disable Multiple Sampling
     */
	ADC12_A_setupSamplingTimer(ADC12_A_BASE,
			ADC12_A_CYCLEHOLD_128_CYCLES,
			ADC12_A_CYCLEHOLD_128_CYCLES,
			ADC12_A_MULTIPLESAMPLESENABLE// For continueous conversion
			);

    /*
     * Base address of the ADC12_A_A Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = AVcc
     * Vr- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
	ADC12_A_configureMemoryParam para;
	para.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	para.inputSourceSelect = ADC12_A_INPUT_A4;
	para.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	para.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	para.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE,&para);

	/* Clear interrupt */
	ADC12_A_clearInterrupt(ADC12_A_BASE,ADC12IFG0);

	/* ADC enable interrupt */
	ADC12_A_enableInterrupt(ADC12_A_BASE,ADC12IE0);
}

#pragma vector=ADC12_VECTOR
__interrupt void Adc_Isr(void)
{
	/* Conversion is completed in channel A0 */
	if(ADC12_A_getInterruptStatus(ADC12_A_BASE,ADC12IFG0))
	{
		adc_value = ADC12_A_getResults(ADC12_A_BASE,ADC12_A_MEMORY_0);
		voltage = (adc_value/4095.0)*3.3;
		//Does not need clear interrupt flag
	}
}
