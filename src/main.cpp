#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include <util/delay.h>
#include "adc_init.h"
#include "timer_init.h"
#include "init_pwm.h"

#define FILTER_LENGHT 10


//globals
int adc_data = 0;
volatile char sample_ready = 0;
const float HAMMING_WINDOW[FILTER_LENGHT] = {0.08, 0.18762, 0.460122, 0.77, 0.972259, 0.972259, 0.77, 0.460122, 0.18762, 0.08};
const float FILTER_COEFF[FILTER_LENGHT] = {-0.02978250913806928037996790692432114156, -0.069911171068381725013374250465858494863,
																					 -0.0710662534165636822214295875710377004, 0.118981592723968451763560949530074140057,
 																				 	  0.362782024568542249554070622252766042948, 0.362782024568542249554070622252766042948,
 																						0.118981592723968451763560949530074140057, -0.0710662534165636822214295875710377004,
																					 -0.069911171068381725013374250465858494863, -0.02978250913806928037996790692432114156 };

//const float HAMMING_WINDOW_TEST[FILTER_LENGHT] = {1,2,3,4,5,6,7,8,9,10};
//const float FILTER_COEFF_TEST[FILTER_LENGHT] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};


/*---------------------------------*/
// RING BUFFER STRUCT
struct ringbuffer{
	float buffer[FILTER_LENGHT] = {0};
	float *head = buffer;
	float *tail = buffer;
	unsigned int count = 0;
};


// -------- FIR FILTER FUNCTIONS --------

struct ringbuffer append_to_ringbuffer(struct ringbuffer ringbuffer, int sample_int){

  // Insert sample at head pointer
  *(ringbuffer.buffer + ringbuffer.count) = (float) sample_int;

  // Increment count, OR reset if it has reached max
  if (ringbuffer.count < FILTER_LENGHT - 1){
    ringbuffer.count++;
  }
  else{
    ringbuffer.count = 0;
  }

  return ringbuffer;
}



float FIR_filter(struct ringbuffer ringbuffer){
	// Delcare output var
	float output = 0;

	// Process
	for (int i = 0; i < FILTER_LENGHT; i++) {
			output += (*(ringbuffer.buffer + i))*FILTER_COEFF[i]*HAMMING_WINDOW[i];
	}

	return output;
}


int main(void)
{
	sei();
	Serial.begin(9600);
	//init_adc();
	//init_timer(169); //frequency
	//init_fastPWM_timer3();

	struct ringbuffer FIR_ringbuffer; // declare ringbuffer struct for FIR filter
	FIR_ringbuffer = append_to_ringbuffer(FIR_ringbuffer, 1);
	Serial.println(FIR_filter(FIR_ringbuffer));

  while(1)
  {

		for (int i = 0; i < 10; i++) {
				FIR_ringbuffer = append_to_ringbuffer(FIR_ringbuffer, 1);
		}
		float filtered_sample = FIR_filter(FIR_ringbuffer);
		Serial.println(filtered_sample);




  }
}



//Service rutinen for TIMER0 interrupt
ISR(TIMER1_COMPB_vect)
{
	TCNT1 = 0;
	//digitalWrite(12, digitalRead(12) ^ 1);
}

//Service rutinen for ADC interrupt
ISR(ADC_vect)
{
	adc_data=ADCL;
	adc_data=(adc_data+(ADCH<<8));
	sample_ready=1;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
