#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "controller.h"
#include "color.h"

#define IDLE_TIMEOUT 1500
#define COLOR_UP     true
#define COLOR_DOWN   false

#define MACHINE_TOGGLE_TIMEOUT 100

// Reasons for entering state
typedef enum
{
    ENTRY,
    CONTINUE,
    EXIT
} Reason;

// IDLE specific variables
uint16_t idle_count = IDLE_TIMEOUT - 1;
bool idle_direction = true;

// ON/OFF mechanics for the state machine
bool machine_toggle = true;
uint16_t toggle_count = 0;

Pers_color pers_color; // Persistent color to save around

// The function pointer that every state must adhere to
typedef void (*State_ptr)(Reason reason, Controller *controller, Animation *anim);

//Points to the current state function
State_ptr curr_state;
State_ptr prev_state;

// Call this to initialize the state machine
void machine_init();

// Used to manage the persistent color
void set_pers_color(Color *color, color_dir dir);
void set_pers_color(uint8_t r, uint8_t g, uint8_t b, color_dir dir);
void clear_pers_color();

// Call this to let the state machine handle the input
bool process_input(Controller *controller, Animation *anim);

// Declaration of the individual states
void state_a(Reason reason, Controller *controller, Animation *anim);
void state_b(Reason reason, Controller *controller, Animation *anim);
void state_z(Reason reason, Controller *controller, Animation *anim);
void state_xy(Reason reason, Controller *controller, Animation *anim);
void state_rl(Reason reason, Controller *controller, Animation *anim);
void state_d_down(Reason reason, Controller *controller, Animation *anim);
void state_smash(Reason reason, Controller *controller, Animation *anim);
void state_directional_b(Reason reason, Controller *controller, Animation *anim);
void state_no_action(Reason reason, Controller *controller, Animation *anim);
void state_no_input(Reason reason, Controller *controller, Animation *anim);

// A function to handle the animation of the idling lights
void idle_lights();

// Determines if the joystick is in smash position
bool is_joy_smash(Controller *controller);
bool is_c_smash(Controller *controller);
bool is_joy_move(Controller *controller);

// Determines if the trigger is in shield position
bool is_rl_shield(Controller *controller);

// A generic stupid state look up, it's awful. fix it
State_ptr generic_state_lookup(Controller *controller);

// Set a flash animation in the animation
void flash_color(Animation *anim, Color *color, uint8_t count);
void flash_color(Animation *anim, uint8_t r, uint8_t g, uint8_t b, uint8_t count);

#include "statemachine.c"

#endif //STATEMACHINE_H
