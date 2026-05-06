#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/unicode.h"

#include <stdexcept>


namespace ViewDesign {


struct KeyEvent {
	u16pair ch;
	uchar key;
	enum : uchar {
		Char,
		KeyDown,
		KeyUp,
		ImeBegin,
		ImeString,
		ImeEnd,
	}type;
};


struct Key {
	enum Value : uchar {
		Undefined = 0x00,

		LButton = 0x01,
		RButton = 0x02,
		Cancel = 0x03,
		MButton = 0x04,
		XButton1 = 0x05,
		XButton2 = 0x06,

		Backspace = 0x08,
		Tab = 0x09,

		Clear = 0x0C,
		Enter = 0x0D,

		Shift = 0x10,
		Ctrl = 0x11,
		Alt = 0x12,
		Pause = 0x13,
		CapsLock = 0x14,

		Escape = 0x1B,

		Space = 0x20,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25,
		Up = 0x26,
		Right = 0x27,
		Down = 0x28,
		Select = 0x29,
		Print = 0x2A,
		Execute = 0x2B,
		PrintScreen = 0x2C,
		Insert = 0x2D,
		Delete = 0x2E,
		Help = 0x2F,

		// 0x30 - 0x39 : Num(0) - Num(9)

		// 0x41 - 0x5A : Char('A') - Char('Z')

		LWin = 0x5B,
		RWin = 0x5C,
		Apps = 0x5D,

		Sleep = 0x5F,

		// 0x60 - 0x69 : Numpad(0) - Numpad(9)

		Multiply = 0x6A,
		Add = 0x6B,
		Separator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,

		// 0x70 - 0x87 : F(1) - F(24)

		NumLock = 0x90,
		ScrollLock = 0x91,

		LShift = 0xA0,
		RShift = 0xA1,
		LCtrl = 0xA2,
		RCtrl = 0xA3,
		LAlt = 0xA4,
		RAlt = 0xA5,
	}value;

	constexpr Key(Value value) : value(value) {}

	constexpr operator uchar() const { return static_cast<uchar>(value); }

	// 'A' - 'Z'
	static constexpr Key Char(char ch) {
		if (ch >= 'A' && ch <= 'Z') {
			return static_cast<Value>(ch);
		}
		throw std::invalid_argument("Char: ch must be an uppercase letter A-Z");
	}

	// 0 - 9
	static constexpr Key Num(int n) {
		if (n >= 0 && n <= 9) {
			return static_cast<Value>('0' + n);
		}
		throw std::invalid_argument("Num: n must be in range 0-9");
	}

	// Numpad 0 - 9
	static constexpr Key Numpad(int n) {
		static constexpr uchar NumpadBase = 0x60; // VK_NUMPAD0
		if (n >= 0 && n <= 9) {
			return static_cast<Value>(NumpadBase + n);
		}
		throw std::invalid_argument("Numpad: n must be in range 0-9");
	}

	// F1 - F24
	static constexpr Key F(int n) {
		static constexpr uchar FBase = 0x70; // VK_F1
		if (n >= 1 && n <= 24) {
			return static_cast<Value>(FBase + n - 1);
		}
		throw std::invalid_argument("F: n must be in range 1-24");
	}
};


} // namespace ViewDesign
