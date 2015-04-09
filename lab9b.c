/********************************************
*
*  Name: Davi Rodrigues Chaves
*  Section: W 3:30-5:00 PM
*  Assignment: Lab 9b - Pulse Width Modulation
*
********************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_ports(void);

volatile char hasChanged;

volatile int var = 47;

int aState, bState, prevAState, prevBState;

ISR(PCINT1_vect) {
    prevAState = aState;
    prevBState = bState;
    if ((PINC & 0b00010000) == 0) { //A5 == 0
        aState = 0;
    } else {
        aState = 1;
    }
    if ((PINC & 0b00100000) == 0) { //A4 == 0
        bState = 0;
    } else {
        bState = 1;
    }
    if (aState != prevAState || bState != prevBState) { // if it changes do something
        hasChanged = 1;
        if (aState == 0 && bState == 0) {
            if (prevAState == 1) {
                var+=1;
            }
            if (prevBState == 1) {
                var-=1;
            }
        }
        if (aState == 0 && bState == 1) {
            if (prevAState == 1) {
                var-=1;
            }
            if (prevBState == 0) {
                var+=1;
            }
        }
        if (aState == 1 && bState == 1) {
            if (prevAState == 0) {
                var+=1;
            }
            if (prevBState == 0) {
                var-=1;
            }
        }
        if (aState == 1 && bState == 0) {
            if (prevAState == 0) {
                var-=1;
            }
            if (prevBState == 1) {
                var+=1;
            }
        }
    }
    if (var < 47) {
        var = 47;
    }
    if (var > 141) {
        var = 141;
    }
}

int main(void) {
    init_ports();
    init_pwm();
    sei(); // Enable interrupts
    /* Main programs goes here */
    while (1) {
        if (hasChanged == 1) {
            OCR1B = var;
            hasChanged = 0;
        }
    } // Loop forever
    return 0;   /* never reached */
}

/*
  init_ports - Do various things to initialize the ports
*/

void init_ports() {
    PCICR |= (1 << PCIE1);
    PCMSK1 |= ((1 << PCINT12)| (1 << PCINT13));
    DDRB |= 0x06; //setting B1-B2 as outputs
    PORTC |= ((1 << PC4) | (1 << PC5));
}

/*
  init_pwm - intitialize 
*/

void init_pwm(void) {
    TCCR1B |= (1 << WGM12); // Set to CTC mode
    TCCR1B |= (1 << WGM13);
    TCCR1A |= (1 << WGM10);
    TCCR1A |= (1 << WGM11);
    TCCR1A |= (1 << COM1B1);
    TCCR1B |= (1 << CS12);
    OCR1A = 1250;
    OCR1B = 47; //(47 -> 0.75 ms and 141 -> 2.25 ms)
}