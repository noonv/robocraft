// измерение температуры термодатчиком:

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "utils/uartstdio.h"

#ifdef DEBUG
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define ADC_SEQUENCER_LENGTH 1

//initializes the adc
void initADC(void) {
	//enable the adc0 peripherial.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	//set the speed to 1msps.
	//SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS);

	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS);

	//set the auto avergage to 64.
	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	//before setting up I must disable the sequence 3.
	ADCSequenceDisable(ADC0_BASE, 3);
	//set the sequence to use (adc0 sequence 3).
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	//set up the sequence step.
	//set up the last step and start an interrupt when the conversion it's over.
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
			ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
	//enable the sequence again!
	ADCSequenceEnable(ADC0_BASE, 3);
}

void initConsole(void) {
	//enable portA
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//configure the pin multiplexing
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	//configure the type of the pins for uart tx/rx
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	//init the console
	UARTStdioInit(0);
}

void initRGBled(void) {
	// Enable PORT F GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// set LED pins as outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED | LED_BLUE | LED_GREEN);
}

int main(void) {

	//Variable to store the data from the sequencer.
	unsigned long ulADC0Value[ADC_SEQUENCER_LENGTH];
	unsigned long ulTemp_ValueC = 0;
	float ulTempValueC = 0;
	//Set the system clock to run at 40MHz.
	SysCtlClockSet(
			SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);
	//I should initialize the adc!
	initADC();
	//init the console for serial use.
	initConsole();

	//initRGBled();

	while (1) {
		// set the red LED pin high, others low
		//GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, LED_RED);
		//SysCtlDelay(SysCtlClockGet() / 24);

		//clear the interrupt flag
		ADCIntClear(ADC0_BASE, 3);
		//trigger the adc conversion process.
		ADCProcessorTrigger(ADC0_BASE, 3);
		//wait for the interrupt flag to get set!
		while (!ADCIntStatus(ADC0_BASE, 3, false)) {
		}
		//get the actual data samples from adc0 sequencer 3!
		ADCSequenceDataGet(ADC0_BASE, 3, ulADC0Value);

		// set the green LED pin high, others low
		//GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, LED_GREEN);

		//convert the value!
		ulTemp_ValueC = ((1475 * 1023) - (2250 * ulADC0Value[0])) / 10230;
		ulTempValueC = (1475 - ((2475 * ulADC0Value[0])) / 4096.)/10.;

		//UARTprintf("Temperature = %3d*C (%d)\n", ulTemp_ValueC, (unsigned long)(ulTempValueC*1000));
		UARTprintf("%d\n", (unsigned long)(ulTempValueC*1000));
		//SysCtlDelay(SysCtlClockGet() / 24);
		SysCtlDelay(SysCtlClockGet() / 12);
	}
}
