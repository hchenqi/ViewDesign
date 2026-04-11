#pragma once

#include "../common/core.h"


namespace ViewDesign {


enum class Key : uchar;


struct KeyEvent {
	enum : uchar {
		Char,
		KeyDown,
		KeyUp,
		ImeBegin,
		ImeString,
		ImeEnd,
	}type;
	Key key;
	wchar ch;
};


enum class Key : uchar {
	Undefined = 0x00,

	LButton = 0x01,
	RButton = 0x02,
	Cancel = 0x03,
	MButton = 0x04,
	XButton1 = 0x05,
	XButton2 = 0x06,

	Backspace = 0x08,  // Back
	Tab = 0x09,

	Clear = 0x0C,
	Enter = 0x0D,  // Return

	Shift = 0x10,
	Ctrl = 0x11,  // Control
	Alt = 0x12,  // Menu
	Pause = 0x13,
	CapsLock = 0x14,  // Capital

	Escape = 0x1B,

	Space = 0x20,
	PageUp = 0x21,  // Prior
	PageDown = 0x22,  // Next
	End = 0x23,
	Home = 0x24,
	Left = 0x25,
	Up = 0x26,
	Right = 0x27,
	Down = 0x28,
	Select = 0x29,
	Print = 0x2A,
	Execute = 0x2B,
	PrintScreen = 0x2C,  // Snapshot
	Insert = 0x2D,
	Delete = 0x2E,
	Help = 0x2F,

	// 0-9  Usage: CharKey('6')

	// A-Z  Usage: CharKey('N')

	LWin = 0x5B,
	RWin = 0x5C,
	Apps = 0x5D,

	Sleep = 0x5F,

	Numpad0 = 0x60,
	Numpad1 = 0x61,
	Numpad2 = 0x62,
	Numpad3 = 0x63,
	Numpad4 = 0x64,
	Numpad5 = 0x65,
	Numpad6 = 0x66,
	Numpad7 = 0x67,
	Numpad8 = 0x68,
	Numpad9 = 0x69,
	Multiply = 0x6A,
	Add = 0x6B,
	Separator = 0x6C,
	Subtract = 0x6D,
	Decimal = 0x6E,
	Divide = 0x6F,
	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,

	NumLock = 0x90,
	ScrollLock = 0x91,  // Scroll

	LShift = 0xA0,
	RShift = 0xA1,
	LCtrl = 0xA2,
	RCtrl = 0xA3,
	LAlt = 0xA4,
	RAlt = 0xA5,
};

constexpr Key CharKey(char ch) {
	if ((ch >= '0' && ch <= '9') ||   // Numbers        : 0x30 - 0x39
		(ch >= 'A' && ch <= 'Z')) {   // Captial letters: 0x41 - 0x5A
		return static_cast<Key>(ch);
	}
	throw std::invalid_argument("ch should be number or captial letter");
}


} // namespace ViewDesign
