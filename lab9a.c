/********************************************
*
*  Name: Davi Rodrigues Chaves
*  Section: W 3:30-5:00 PM
*  Assignment: Lab 9a - Pulse Width Modulation
*
********************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_ports(void);

volatile char hasChanged;

int var = 0;

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
}

int main(void) {
    init_ports();
    init_pwm();
    var = OCR0A;
    sei(); // Enable interrupts
    /* Main programs goes here */
    while (1) {
        if (hasChanged == 1) {
            OCR0A = var;
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
    DDRD |= 0xF0; //setting D4-D7 as outputs
    DDRB |= 0x03; //seeting B1-B2 as outputs
    PORTC |= ((1 << PC4) | (1 << PC5));
}

/*
  init_pwm - intitialize 
*/

void init_pwm(void) {
    TCCR0A |= (0b11 << WGM00); // Fast PWM mode, modulus = 256
    TCCR0A |= (0b10 << COM0A0); // Turn D6 on at 0x00 and off at OCR0A
    OCR0A = 128; // Initial pulse duty cycle of 50%
    TCCR0B |= (0b101 << CS00); // Prescaler = 1024 => 16ms period
}