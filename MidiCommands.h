#ifndef MM_MIDI_COMMANDS
#define MM_MIDI_COMMANDS

#define MM_STATUS_MASK      0xf0
#define MM_CHANNEL_MASK     0x0f

#define MM_NOTE_OFF         0x80
#define MM_NOTE_ON          0x90
#define MM_AFTERTOUCH       0xA0
#define MM_CONTROL_CHANGE   0xB0
#define MM_PATCH_CHANGE     0xC0
#define MM_CHANNEL_PRESSURE 0xD0
#define MM_PITCH_WHEEL      0xE0
#define MM_SYSTEM_EXCLUSIVE 0xF0

#define MM_MAX_ATTACK       0x7f
#define MM_MIN_ATTACK       0x00

#define MM_CONTROL_TIMBRE       71
#define MM_CONTROL_BRIGHTNESS   74

#endif