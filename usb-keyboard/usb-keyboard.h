#define HWSERIAL Serial1
#include "utils.h"

#define MYDEBUG
#define KBD_BUFFSZ 200
#define KEYNAME_BUFFSZ 25
#define PREFIX 17 // CTRL-Q

// Util functions
uint16_t escape_sequence_to_keycode(char in_ascii);
uint16_t special_char_to_keycode(char in_ascii);
uint16_t keyname_to_keycode(const char* keyname);
uint16_t parse_keyname(const char* keyname);
void usb_send_key(uint16_t key, uint16_t mod);

// Interactive mode functions
void interactive_mode(char in_ascii);

// Command mode functions
void command_mode(char in_ascii);
void c_parse(char* str);
void c_sendraw(char* pch);
void c_send(char* pch);
void c_unicode(char* pch, bool linux);
void c_sleep(char* pch);
