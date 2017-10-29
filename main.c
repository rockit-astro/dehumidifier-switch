//*********************************************************************************
//  Copyright 2017 Paul Chote
//  This file is part of dehumidifier-switch, which is free software. It is made
//  available to you under version 3 (or later) of the GNU General Public License,
//  as published by the Free Software Foundation and included in the LICENSE file.
//*********************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "usb.h"

#define IO_PORT PORTD
#define IO_PINREG PIND
#define IO_DDR DDRD
#define SWITCH_PIN PD2
#define SWITCH_DD DDD2
#define RELAY_PIN PD4
#define RELAY_DD DDD4

volatile bool relay_enabled;
volatile bool toggle_lights;

void tick()
{
    while (usb_can_read())
    {
        uint8_t value = usb_read();
        
        // Last bit sets the relay state
        if (value & 0x01)
        {
            IO_PORT |= _BV(RELAY_PIN);
            relay_enabled = true;
        }
        else
        {
            IO_PORT &= ~_BV(RELAY_PIN);
            relay_enabled = false;
        }
    }
}

int main()
{
    // Set Relay as output
    IO_DDR = _BV(RELAY_DD);

    // Enable pullup on switch input
    IO_PORT = _BV(SWITCH_PIN);

    // Configure timer1 to interrupt every 0.5 seconds for status updates
    OCR1A = 7812;
    TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12);
    TIMSK1 |= _BV(OCIE1A);

    // Configure timer0 to interrupt after 1.024ms for debouncing the light switch
    TCCR0A = _BV(WGM01);
    OCR0A = 16;    
    TIMSK0 |= _BV(OCIE0A);

    // Enable rising edge trigger for switch pin
    EIMSK = _BV(INT0);
    EICRA = _BV(ISC01);

    usb_initialize();

    sei();
    for (;;)
        tick();
}

// Button pressed
ISR(INT0_vect)
{
    // Start button debounce timer
    TCCR0B = _BV(CS02) | _BV(CS00);
}

// Button press debounce timer elapsed
ISR(TIMER0_COMPA_vect)
{
    TCCR0B = 0;
    if (bit_is_clear(PIND, PD6))
        toggle_lights = true;
}

// Status update interrupt
ISR(TIMER1_COMPA_vect)
{
    uint8_t status = (toggle_lights ? 0x02 : 0x0) | (relay_enabled ? 0x01 : 0x00);
    toggle_lights = false;
    usb_write(status);
}
