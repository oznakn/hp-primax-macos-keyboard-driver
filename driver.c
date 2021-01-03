#include <stdio.h>
#include <stdlib.h>

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#include "hidapi.h"

#define VENDOR_ID 0x0461 // primax
#define PRODUCT_ID 0x4e90

/*
	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);
*/

const static int keys[] = {
	kVK_ANSI_A,
	kVK_ANSI_B,
	kVK_ANSI_C,
	kVK_ANSI_D,
	kVK_ANSI_E,
	kVK_ANSI_F,
	kVK_ANSI_G,
	kVK_ANSI_H,
	kVK_ANSI_I,
	kVK_ANSI_J,
	kVK_ANSI_K,
	kVK_ANSI_L,
	kVK_ANSI_M,
	kVK_ANSI_N,
	kVK_ANSI_O,
	kVK_ANSI_P,
	kVK_ANSI_Q,
	kVK_ANSI_R,
	kVK_ANSI_S,
	kVK_ANSI_T,
	kVK_ANSI_U,
	kVK_ANSI_V,
	kVK_ANSI_W,
	kVK_ANSI_X,
	kVK_ANSI_Y,
	kVK_ANSI_Z,
	kVK_ANSI_1,
	kVK_ANSI_2,
	kVK_ANSI_3,
	kVK_ANSI_4,
	kVK_ANSI_5,
	kVK_ANSI_6,
	kVK_ANSI_7,
	kVK_ANSI_8,
	kVK_ANSI_9,
	kVK_ANSI_0,
	kVK_Return,
	kVK_Escape,
	kVK_Delete,
	kVK_Tab,
	kVK_Space,
	kVK_ANSI_Minus,
	kVK_ANSI_Equal,
	kVK_ANSI_LeftBracket,
	kVK_ANSI_RightBracket,
	-1, // empty
	kVK_ANSI_Backslash,
	kVK_ANSI_Semicolon,
	kVK_ANSI_Quote,
	kVK_ISO_Section, // 102ND
	kVK_ANSI_Comma,
	kVK_ANSI_Period,
	kVK_ANSI_Slash,
	kVK_CapsLock,
	kVK_F1,
	kVK_F2,
	kVK_F3,
	kVK_F4,
	kVK_F5,
	kVK_F6,
	kVK_F7,
	kVK_F8,
	kVK_F9,
	kVK_F10,
	kVK_F11,
	kVK_F12,
	-1, // print screen
	-1, // scroll lock
	-1, // pause
	kVK_Help, // insert/help
	kVK_Home,
	kVK_PageUp,
	kVK_ForwardDelete,
	kVK_End,
	kVK_PageDown,
	kVK_RightArrow,
	kVK_LeftArrow,
	kVK_DownArrow,
	kVK_UpArrow,
	kVK_ANSI_KeypadClear, // num lock
	kVK_ANSI_KeypadDivide,
	kVK_ANSI_KeypadMultiply,
	kVK_ANSI_KeypadMinus,
	kVK_ANSI_KeypadPlus,
	kVK_ANSI_KeypadEnter,
	kVK_ANSI_Keypad1,
	kVK_ANSI_Keypad2,
	kVK_ANSI_Keypad3,
	kVK_ANSI_Keypad4,
	kVK_ANSI_Keypad5,
	kVK_ANSI_Keypad6,
	kVK_ANSI_Keypad7,
	kVK_ANSI_Keypad8,
	kVK_ANSI_Keypad9,
	kVK_ANSI_Keypad0,
	kVK_ANSI_KeypadDecimal,
	kVK_ANSI_Grave // < >
};

static volatile int run = 1;

static int key_counter = 0;
static int last_key = 0;

void sig_int_handle(int a) {
	run = 0;
}

int main(void) {
	unsigned char buf[3] = {0};

	signal(SIGINT, sig_int_handle);

	hid_init();

	hid_device *handle = hid_open(VENDOR_ID, PRODUCT_ID, NULL);

	while (run) {
		hid_read_timeout(handle, buf, 3, 200);

		if (last_key != buf[2] || key_counter >= 2) {
			register int pre_key = buf[0];
			register int key = buf[2];

			// printf("%d %d\n", buf[0], buf[2]);

			if (key != 0) {
				register int key_code = keys[key - 4];

				if (key_code != -1) {
					CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
					CGEventRef press = CGEventCreateKeyboardEvent(source, key_code, true);

					if (pre_key & 1 || pre_key & (1 << 4)) {
						CGEventSetFlags(press, kCGEventFlagMaskControl);
					}
					else if (pre_key & (1 << 1) || pre_key & (1 << 5)) {
						CGEventSetFlags(press, kCGEventFlagMaskShift);
					}
					else if (pre_key & (1 << 3) || pre_key & (1 << 6)) { // revert left alt with cmd, << 2
						CGEventSetFlags(press, kCGEventFlagMaskAlternate);
					}
					else if (pre_key & (1 << 2) || pre_key & (1 << 7)) { // revert left cmd with alt, << 3
						CGEventSetFlags(press, kCGEventFlagMaskCommand);
					}

					CGEventRef release = CGEventCreateKeyboardEvent(source, key_code, false);

					CGEventPost(kCGAnnotatedSessionEventTap, press);
					CGEventPost(kCGAnnotatedSessionEventTap, release);

					CFRelease(release);
					CFRelease(press);
					CFRelease(source);
				}
			} else {
				key_counter = 0;
			}

			last_key = buf[2];
		} else {
			key_counter++;
		}
	}

	hid_close(handle);

	hid_exit();

	return 0;
}
