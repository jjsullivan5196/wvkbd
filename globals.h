#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>

extern uint32_t get_current_height();
void set_current_height(uint32_t height);

extern void handle_command(uint32_t code);

extern struct clr_scheme schemes[];

#endif // GLOBALS_H_
