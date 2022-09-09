///////////////////////////////////////////////////////////////////////////////
// Sample　Code : Setting Up Ultrasonic Sensor Echo Operation in Interrupt Mode
//
// Author: Bill Stefanuk
// 
// Note: This is an INCOMPLETE code for Ultrasonic Sensor initialization.
//       It is meant to be an example of how to use NVIC API to enable the interrupt.
//       Straight-compiling this code will result in compilation faiure.
//
///////////////////////////////////////////////////////////////////////////////


//Echo input
//Adjust prescaler so clock is 1MHz will return in micro seconds
//measure the echo pulse width in ISR

// IDEA: make trigger freq a parameter

void UltrasonicEcho_Init(){

//*************************************************************************************	
// configure GPIO port - PC.7 AF2 TIM3, CH2 
	// turn on clock to GPIOC 	
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	
	
	// set 7 pin mode AF - AF2 = TIM3,Ch2
#define AF2 2	
	GPIO_PIN_MODE( C, UDS_ECHO_PIN, 2UL );
	FORCE_BITS( GPIOC->AFR[0], GPIO_AFRL_AFRL7, AF2<<GPIO_AFRL_AFRL7_Pos );	// *MUST* be better way to do this

	//Set PC.7 to No Pull-up/Pull-down
	GPIO_PIN_PUPDR( C, UDS_ECHO_PIN, 0UL );
	
//*************************************************************************************	
// configure TIM3 to count in usec	
	
	// Enable the clock of timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Set up an appropriate prescaler so counter counts in usec
	TIM3->PSC = SystemCoreClock/(1000000UL) - 1;
	
	// Set ARR to max value
	//TIM3->ARR = 0xFFFF;
	TIM3->ARR=0xFFFF;  //WPS   when FFFF, often got large incorrect readings near the max
	
	//	Set TIM3 counting direction to up-counting
	CLR_BITS(TIM3->CR1, TIM_CR1_DIR);

	// Make TIM3 Channel 2 input channel.  Connect to CCR2
	//FORCE_BITS( TIM3->CCMR1, TIM_CCMR1_CC2S, TIM_CCMR1_CC2S_0 );	// T3-Ch2 connected to CCR2 !!!

	// Program the input filter duration
	// Set as no filter by clearing ICxF[3:0] bits (input capture 1 filter)
	//CLR_BITS( TIM3->CCMR1, TIM_CCMR1_IC2F );
	//FORCE_BITS( TIM3->CCMR1, TIM_CCMR1_IC2F, 5 << TIM_CCMR1_IC2F_Pos );
	
	// Program the input prescaler
	// to capture each valid transition, set the input prescaler to zero;
	// ICxPSC[1:0] bits (inputs capture 1 prescaler)
	//CLR_BITS( TIM3->CCMR1, TIM_CCMR1_IC2PSC );	

//****************************
// master config for CCMR1 - both CCR1 & CCR2 look at input from channel 2 pin
TIM3->CCMR1 = 0x102;
//****************************
	
	// set falling edges passed to the triggers circuits - rising resets counter, falling captures value
		//TIM3->CCER |= (TIM_CCER_CC2P | TIM_CCER_CC2NP); // WPS
//		TIM3->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
//		TIM3->CCER |= TIM_CCER_CC2P;

//****************************
// master config for CCER - CCR1 looks at rising edge of input, CCR2 looks at falling edge
TIM3->CCER = 0x02;   // WPS

//****************************
	
//	// select input trigger source  (Fix this!!!! - works but ugly)
//	FORCE_BITS( TIM3->SMCR, TIM_SMCR_TS, 6<<4 );  // TI1   WPS

//	// Select RESET slave mode
//	FORCE_BITS( TIM3->SMCR, TIM_SMCR_SMS, 4 ); 
	
//****************************
// master config for SMCR - trigger is channel 2 input, reset mode
TIM3->SMCR = 0x64;
//****************************

// Enable capture of counter
TIM3->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

	//*************************************************************************************	
	// Configure to generate interrupt on each pulse measured
	
//	// Allow TIM3, Ch2 to generate interrupt request (IRQ)
//	SET_BITS( TIM3->DIER, TIM_DIER_CC2IE );
//	
//	// Set priority of TIM3 IRQ to maximum (for no apparent reason)
//	NVIC_SetPriority( TIM3_IRQn, 0 );
//	
//	// Enable IRQ response by NVIC
//	NVIC_EnableIRQ( TIM3_IRQn );

	// Allow TIM3, Ch2 to generate interrupt request (IRQ)
	SET_BITS( TIM3->DIER, TIM_DIER_CC1IE );
	
	// Set priority of TIM3 IRQ to maximum (for no apparent reason)
	NVIC_SetPriority( TIM3_IRQn, 0 );
	
	// Enable IRQ response by NVIC
	NVIC_EnableIRQ( TIM3_IRQn );

	// Enable the counter
	TIM3->CR1 |= TIM_CR1_CEN;

} // UltrasonicEcho_Init()


	//	how to present when no target (what will happen - different interrupt?)
	//	write interrupt handler

// need mode on counter where it stops counting when

void TIM3_IRQHandler( void ) {

	// if there was ALSO a timer overflow event, the timer wrapped and the measured pulse width is invalid
//	if ( BIT_IS_SET( TIM3->SR, TIM_SR_UIF ) ) {
//			CLR_BITS( TIM3->SR, TIM_SR_UIF );
//	}		
	
	// if CC2 event triggered IRQ, CCR2 is the pulse width in us
	if ( BIT_IS_SET( TIM3->SR, TIM_SR_CC1IF ) ) {
			Green_LED_Toggle();
			ultrasonicPulseWidth = TIM3->CCR1;	// legit echo detected - clears the pending flag
	}	
//	

	
} // TIM3_IRQHandler()