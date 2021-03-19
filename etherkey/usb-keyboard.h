#pragma once

#define HWSERIAL Serial1
#include "utils.h"

#define MYDEBUG
#define KBD_BUFFSZ 200
#define KEYNAME_BUFFSZ 25
#define PREFIX 17 // CTRL-Q

// Util functions
int mode_select(char in_ascii, int oldmode);
uint16_t escape_sequence_to_keycode(char in_ascii);
uint16_t special_char_to_keycode(char in_ascii);
uint16_t keyname_to_keycode(const char* keyname);
void usb_send_key(uint16_t key, uint16_t mod);

// Interactive mode functions
void interactive_mode(char in_ascii);

// Command mode functions
void command_mode(char in_ascii);
void c_parse(char* str);
bool c_parse_ext(char* str, bool send_single, int modifier);
void c_sendraw(char* pch);
void c_send(char* pch);
void c_unicode(char* pch, bool linux);
void c_sleep(int ms);

// Debug mode functions
void debug_mode(char in_ascii);
