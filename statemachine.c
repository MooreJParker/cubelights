#include "statemachine.h"

#include <stdlib.h>

void machine_init()
{
    curr_state = &state_a;
    prev_state  = &state_no_input;
    clear_pers_color(); }

void set_pers_color(Color *color, color_dir dir = COLOR_UP)
{
    pers_color.color.r = color->r;
    pers_color.color.g = color->g;
    pers_color.color.b = color->b;
    pers_color.color.w = color->w;

    pers_color.dir = dir;
}

void set_pers_color(uint8_t r, uint8_t g, uint8_t b, color_dir dir = COLOR_UP)
{
    pers_color.color.r = r;
    pers_color.color.g = g;
    pers_color.color.b = b;
    pers_color.color.w = MAX_BRIGHTNESS;
    
    pers_color.dir = dir;
}

void clear_pers_color() {
    pers_color.color.r = 0;
    pers_color.color.g = 0;
    pers_color.color.b = 0;
    pers_color.color.w = 0;

    pers_color.dir = COLOR_UP;
}

bool process_input(Controller *controller, Animation *anim)
{
    curr_state = generic_state_lookup(controller);
    if(curr_state != prev_state)
    {
        prev_state(EXIT, controller, anim); 
        curr_state(ENTRY, controller, anim); 
    }
    else
    {
        curr_state(CONTINUE, controller, anim);
    }
    prev_state = curr_state;
    return true;
}

/************************************************
                State Implemenations
************************************************/
void state_a(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
    case ENTRY:
    case CONTINUE:
        set_next_color(anim, 0, 255, 0);
        break;
    case EXIT:
        //intentionally left blank
        break;
    }
}

void state_b(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
    case ENTRY:
    case CONTINUE:
        set_next_color(anim, 255, 0, 0);
        break;
    case EXIT:
        //intentionally left blank
        // The Joey Special
        if(curr_state != state_directional_b)
        {
            for(uint8_t i = 0; i < 9; i++)
            {
                uint8_t rand_r = rand() % (255 + 1);
                uint8_t rand_g = rand() % (255 + 1);
                uint8_t rand_b = rand() % (255 + 1);
                set_next_color(anim, rand_r, rand_g, rand_b);
                set_next_color(anim, 0, 0, 0);
            }
        }
        break;
    }
}

void state_z(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
    case ENTRY:
    case CONTINUE:
        set_next_color(anim, 200, 0, 255);
        break;
    case EXIT:
        //intentionally left blank
        break;
    }
}

void state_xy(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
    case ENTRY:
    case CONTINUE:
        set_next_color(anim, 255, 200, 0);
        break;
    case EXIT:
        // intentionally left blank
        break;
    }
}

void state_rl(Reason reason, Controller *controller, Animation *anim)
{
    static const uint8_t shield_min = 35;
    static const uint8_t shield_max = 255;
    static const uint8_t incr = 5;

    switch(reason)
    {
        case ENTRY:
        {
            // Set the animation
            set_first_color(anim, 0, 0, shield_min);

            // This is the start of an interruptable animation
            set_pers_color(0, 0, shield_min);
            break;
        }
        case CONTINUE:
            // Continuation of an interruptabel state driven animation
            if(COLOR_UP == pers_color.dir)
            {
                if(pers_color.color.b + incr >= shield_max)
                {
                    set_pers_color(pers_color.color.r, pers_color.color.g, pers_color.color.b - incr, COLOR_DOWN);
                }
                else
                {
                    set_pers_color(pers_color.color.r, pers_color.color.g, pers_color.color.b + incr, COLOR_UP);
                }
            }
            else
            {
                if(pers_color.color.b - incr <= shield_min)
                {
                    set_pers_color(pers_color.color.r, pers_color.color.g, pers_color.color.b + incr, COLOR_UP);
                }
                else
                {
                    set_pers_color(pers_color.color.r, pers_color.color.g, pers_color.color.b - incr, COLOR_DOWN);
                }
            }
            set_next_color(anim, pers_color.color.r, pers_color.color.g, pers_color.color.b);
            break;
        case EXIT:
            // Intentionally left blank
            break;
    }
}

void state_d_down(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
        case ENTRY:
            // start the on/off toggle
            toggle_count = 0;
            break;
        case CONTINUE:
            if(toggle_count < MACHINE_TOGGLE_TIMEOUT)
            {
                // we just hit the toggle limit
                if(toggle_count + 1 == MACHINE_TOGGLE_TIMEOUT)
                {
                    machine_toggle = !machine_toggle;
                    if(machine_toggle)
                    { // Green for machine being turned on
                        flash_color(anim, 0, 255, 0, 4);
                    }
                    else
                    { // Red for machine being turned off
                        flash_color(anim, 255, 0, 0, 4);
                    }
                }
                ++toggle_count;
            }
            break;
        case EXIT:
            toggle_count = 0;
            break;
    }
}

void state_smash(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
        case ENTRY:
        {
            Color color = new_color(0, 0, 0);

            // Set the animation
            set_first_color(anim, 0, 0, 0);

            // This is the start of an interruptable animation
            set_pers_color(&color);
            break;
        }
        case CONTINUE:
        {
            // Continuation of an interruptable state driven animation
            if(pers_color.color.g > 40 && pers_color.color.g < 255)
            {
                set_next_color(anim, pers_color.color.r, ++pers_color.color.g, ++pers_color.color.b);
                anim->delay = 10;
            }
            else if(pers_color.color.g < 255)
            {
                ++pers_color.color.g;
            }
            break;
        }
        case EXIT:
        {
            if(pers_color.color.g > 40)
            {
                // Exit animation to flash the lights
                flash_color(anim, 0, 255, 255, 5);
            }
            else
            {
                set_next_color(anim, 0, 255, 0);
                set_next_color(anim, 0, 254, 0);
                anim->delay = 50;
            }
            break;
        }
    }
}
void state_directional_b(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
        case ENTRY:
            break;
        case CONTINUE:
            set_first_color(anim, 255, 0, 0);
            anim->delay = 250;
            break;
        case EXIT:
            //intentionally left blank
            break;
    }
}

void state_no_action(Reason reason, Controller *controller, Animation *anim)
{
    //do nothing but interrupt the idle animation
}

void state_no_input(Reason reason, Controller *controller, Animation *anim)
{
    switch(reason)
    {
        case ENTRY:
            // Start the idle count
            ++idle_count;
            break;
        case CONTINUE:
            //Check to see if we're already idle
            if(idle_count < IDLE_TIMEOUT)
            {
                if(idle_count + 1 == IDLE_TIMEOUT)
                { // We just started out idle
                    idle_direction = COLOR_UP;
                    set_pers_color(25, 25, 25);
                }
                ++idle_count;
                set_next_color(anim, 0, 0, 0);
            }
            else
            {
                idle_lights();
                set_next_color(anim, pers_color.color.r, 0, (pers_color.color.r * .25));
                anim->delay = 2;
            }
            break;
        case EXIT:
            idle_count = 0;
            break;
            
    }
}
/******************************************************************
                    END STATES
******************************************************************/

State_ptr generic_state_lookup(Controller *controller)
{
    if(CONTROLLER_A(*controller))
    {
        if(is_joy_smash(controller))
        {
            return &state_smash;
        }
        return &state_a;
    }
    else if(CONTROLLER_B(*controller))
    {
        if(is_joy_smash(controller))
        {
            return &state_directional_b;
        }
        return &state_b;
    }
    else if(CONTROLLER_X(*controller) || CONTROLLER_Y(*controller))
    {
        return &state_xy;
    }
    else if(CONTROLLER_Z(*controller))
    {
        return &state_z;
    }
    else if(CONTROLLER_D_DOWN(*controller))
    {
        return &state_d_down;
    }
    else if(is_rl_shield(controller) || CONTROLLER_R(*controller) || CONTROLLER_L(*controller))
    {
        return &state_rl;
    }
    else if(is_c_smash(controller))
    {
        return &state_smash;
    }
    else if(is_joy_move(controller)         ||
            CONTROLLER_START(*controller)   ||
            CONTROLLER_D_UP(*controller)    ||
            CONTROLLER_D_RIGHT(*controller) ||
            CONTROLLER_D_LEFT(*controller)  )
    {
        return &state_no_action;
    }
    else
    {
        return &state_no_input;
    }
}

bool is_c_smash(Controller *controller)
{
    return controller->c_x > C_STICK_TOP_THRESHOLD    ||
           controller->c_x < C_STICK_BOTTOM_THRESHOLD ||
           controller->c_y > C_STICK_TOP_THRESHOLD    ||
           controller->c_y < C_STICK_BOTTOM_THRESHOLD;
}

bool is_joy_smash(Controller *controller)
{
    return controller->joy_x > SMASH_TOP_THRESHOLD - 55    ||
           controller->joy_x < SMASH_BOTTOM_THRESHOLD + 55 ||
           controller->joy_y > SMASH_TOP_THRESHOLD - 55    ||
           controller->joy_y < SMASH_BOTTOM_THRESHOLD + 55; 
}

bool is_joy_move(Controller *controller)
{
    return controller->joy_x > C_STICK_TOP_THRESHOLD    ||
           controller->joy_x < C_STICK_BOTTOM_THRESHOLD ||
           controller->joy_y > C_STICK_TOP_THRESHOLD    ||
           controller->joy_y < C_STICK_BOTTOM_THRESHOLD;
}

bool is_rl_shield(Controller *controller)
{
    return controller->analog_r > SHIELD_THRESHOLD || 
           controller->analog_l > SHIELD_THRESHOLD; 
}

void idle_lights()
{
    if(idle_direction == COLOR_UP)
    {
        if(pers_color.color.r == 255)
        {
            idle_direction = COLOR_DOWN;
        }
        else
        {
            pers_color.color.r += 1;
            pers_color.color.g += 1;
            pers_color.color.b += 1;
        }
    } 
    else
    {
        if(pers_color.color.r == 25)
        {
            idle_direction = COLOR_UP;
        }
        else
        {
            pers_color.color.r -= 1;
            pers_color.color.g -= 1;
            pers_color.color.b -= 1;
        }
    }
}

void flash_color(Animation *anim, Color *color, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        set_next_color(anim, color->r, color->g, color->b); 
        set_next_color(anim, 0, 0, 0); 
    }
}

void flash_color(Animation *anim, uint8_t r, uint8_t g, uint8_t b, uint8_t count)
{
    Color color = new_color(r, g, b);
    flash_color(anim, &color, count);
}
