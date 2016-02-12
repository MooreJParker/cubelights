#define F_CPU 16000000UL // 16 MHZ
//#define F_CPU 8000000UL // 8 MHZ
//#define F_CPU 1000000UL // 1 MHZ

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "libs/Neopixel.h"
#include "controller.h"
#include "color.h"
#include "statemachine.h"

#define GET_BIT(TGT, PIN) ((TGT) & (1 << (PIN)))
#define SET_BIT(TGT, PIN) do { TGT |= (1 << (PIN)); } while(0)
#define CLEAR_BIT(TGT, PIN) do { TGT &= ~(1 << (PIN)); } while(0)
#define TOGGLE_BIT(TGT, PIN) do { TGT ^= (1 << (PIN)); } while(0)

void setup_pins(void) {
    CLEAR_BIT(DDRB, PB1); // Set PB1(AIN1) as input, GCN data signal
    SET_BIT(PORTB, PB1);  // Enable pull-up resistor on PB1
    SET_BIT(DDRB, PB3);   // Set PB3 as output, debug LED
}

void setup_comparator(void) {
    SET_BIT(ACSR, ACBG);        // Enable 1.1V positive input reference voltage
}

void setup_timer0(void) {
    SET_BIT(TCCR0B, CS00);
}

uint8_t wait_amount = 16;

bool getMessage(uint8_t message_buffer[]) {
    // Zero out input array
    for(int i = 0; i < 12; ++i) {
        message_buffer[i] = 0x00;
    }

    // Wait for first bit
    while(!GET_BIT(ACSR, ACO)) {}
    while(GET_BIT(ACSR, ACO)) {}

    for(uint8_t cur_byte = 0; cur_byte < 11; cur_byte++) {
        for(uint8_t bitmask = 0x80; bitmask; bitmask >>= 1) {
            // Reset timer
            TCNT0 = 0;
            // Wait for signal to go low
            while(!GET_BIT(ACSR, ACO)) {
                if(TCNT0 >= 240)    // Timeout
                    return false;
            }

            // Reset timer and wait for signal's critical point
            TCNT0 = 0;
            while(TCNT0 <= wait_amount) {}

            // Check if signal is high
            if(!GET_BIT(ACSR, ACO)) {
                message_buffer[cur_byte] |= bitmask;
            }

            // Make sure the signal is high before looping
            while(GET_BIT(ACSR, ACO)) {
                if(TCNT0 >= 240)
                    return false;   // Timeout
            }

            // Adjust wait time to be a half-period
            wait_amount = TCNT0 / 2;
        }
    }
    return true;
}

void delay_ms(uint16_t ms)
{
    while(ms > 0)
    {
        _delay_ms(1);
        ms--;
    }
}

// This is over complicated
void show_animation(Animation *animation)
{
    // Kind of odd, but ok
    if(0 == animation->count)
    {
        return;
    }

    // If it's just a single color, show it and return
    if(1 == animation->count)
    {
        showColor(animation->color[0].r, animation->color[0].g, animation->color[0].b, animation->color[0].w);
        return;
    }

    for(uint8_t i = 0; i < animation->count - 1; i++)
    {
        Color curr = animation->color[i];
        Color next = animation->color[i + 1];
        bool not_done = true;
        while(not_done)
        {
            bool r_done = false;
            bool g_done = false;
            bool b_done = false;
            if(!r_done)
            {
                if(curr.r < next.r)
                    curr.r++;
                else if(curr.r > next.r)
                    curr.r--;
                else
                    r_done = true;
            }
            if(!g_done)
            {
                if(curr.g < next.g)
                    curr.g++;
                else if(curr.g > next.g)
                    curr.g--;
                else
                    g_done = true;
            }
            if(!b_done)
            {
                if(curr.b < next.b)
                    curr.b++;
                else if(curr.b > next.b)
                    curr.b--;
                else
                    b_done = true;
            }
            if(r_done && g_done && b_done)
            {
                not_done = false;
            }
            showColor(curr.r, curr.g, curr.b, curr.w);
            delay_ms(animation->delay);
        }
    }
    
}

int main(void)
{
    setup_pins();
    setup_comparator();
    setup_timer0();

    ledsetup();
    uint8_t message_buffer[12] = {0};
    Controller *controller = (Controller*)message_buffer;
    machine_init();

    Animation anim;
    clear_animation(&anim);
    bool prev_machine_toggle = true;

    delay_ms(1000);

    while(1)
    {
        // Wait until we have a valid message
        while(!getMessage(message_buffer)) {}

        // Interpret the input
        process_input(controller, &anim); 

        if((prev_machine_toggle && !machine_toggle) ||
            machine_toggle)
        { 
            show_animation(&anim);
        }
        clear_animation(&anim);
        prev_machine_toggle = machine_toggle;

        // Wait for the 2nd paired request to pass
        _delay_us(2000);
    }
}
