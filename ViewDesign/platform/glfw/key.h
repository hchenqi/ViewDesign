#pragma once

#include "ViewDesign/event/key_event.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {


constexpr Key AsKey(int key) {
	if (key >= 'A' && key <= 'Z') { return Key::Char(static_cast<char>(key)); }

	if (key >= '0' && key <= '9') { return Key::Num(key - '0'); }

	if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) { return Key::Numpad(key - GLFW_KEY_KP_0); }

	if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F24) { return Key::F(key - GLFW_KEY_F1 + 1); }
	// GLFW_KEY_F25: no VK_F25, falls through to Undefined

	switch (key) {
	case GLFW_KEY_SPACE:         return Key::Space;

	case GLFW_KEY_APOSTROPHE:    break; // VK_OEM_7    — layout-dependent
	case GLFW_KEY_COMMA:         break; // VK_OEM_COMMA
	case GLFW_KEY_MINUS:         break; // VK_OEM_MINUS
	case GLFW_KEY_PERIOD:        break; // VK_OEM_PERIOD
	case GLFW_KEY_SLASH:         break; // VK_OEM_2

		// GLFW_KEY_0 - GLFW_KEY_9

	case GLFW_KEY_SEMICOLON:     break; // VK_OEM_1
	case GLFW_KEY_EQUAL:         break; // VK_OEM_PLUS

		// GLFW_KEY_A - GLFW_KEY_Z

	case GLFW_KEY_LEFT_BRACKET:  break; // VK_OEM_4
	case GLFW_KEY_BACKSLASH:     break; // VK_OEM_5
	case GLFW_KEY_RIGHT_BRACKET: break; // VK_OEM_6
	case GLFW_KEY_GRAVE_ACCENT:  break; // VK_OEM_3
	case GLFW_KEY_WORLD_1:       break; // no Win32 equivalent
	case GLFW_KEY_WORLD_2:       break; // no Win32 equivalent

	case GLFW_KEY_ESCAPE:        return Key::Escape;
	case GLFW_KEY_ENTER:         return Key::Enter;
	case GLFW_KEY_TAB:           return Key::Tab;
	case GLFW_KEY_BACKSPACE:     return Key::Backspace;
	case GLFW_KEY_INSERT:        return Key::Insert;
	case GLFW_KEY_DELETE:        return Key::Delete;

	case GLFW_KEY_RIGHT:         return Key::Right;
	case GLFW_KEY_LEFT:          return Key::Left;
	case GLFW_KEY_DOWN:          return Key::Down;
	case GLFW_KEY_UP:            return Key::Up;

	case GLFW_KEY_PAGE_UP:       return Key::PageUp;
	case GLFW_KEY_PAGE_DOWN:     return Key::PageDown;
	case GLFW_KEY_HOME:          return Key::Home;
	case GLFW_KEY_END:           return Key::End;

	case GLFW_KEY_CAPS_LOCK:     return Key::CapsLock;
	case GLFW_KEY_SCROLL_LOCK:   return Key::ScrollLock;
	case GLFW_KEY_NUM_LOCK:      return Key::NumLock;
	case GLFW_KEY_PRINT_SCREEN:  return Key::PrintScreen;
	case GLFW_KEY_PAUSE:         return Key::Pause;

		// GLFW_KEY_F1 - GLFW_KEY_F25

		// GLFW_KEY_KP_0 - GLFW_KEY_KP_9

	case GLFW_KEY_KP_DECIMAL:    return Key::Decimal;
	case GLFW_KEY_KP_DIVIDE:     return Key::Divide;
	case GLFW_KEY_KP_MULTIPLY:   return Key::Multiply;
	case GLFW_KEY_KP_SUBTRACT:   return Key::Subtract;
	case GLFW_KEY_KP_ADD:        return Key::Add;
	case GLFW_KEY_KP_ENTER:      return Key::Enter;
	case GLFW_KEY_KP_EQUAL:      break; // no standard Win32 VK

	case GLFW_KEY_LEFT_SHIFT:    return Key::LShift;
	case GLFW_KEY_LEFT_CONTROL:  return Key::LCtrl;
	case GLFW_KEY_LEFT_ALT:      return Key::LAlt;
	case GLFW_KEY_LEFT_SUPER:    return Key::LWin;

	case GLFW_KEY_RIGHT_SHIFT:   return Key::RShift;
	case GLFW_KEY_RIGHT_CONTROL: return Key::RCtrl;
	case GLFW_KEY_RIGHT_ALT:     return Key::RAlt;
	case GLFW_KEY_RIGHT_SUPER:   return Key::RWin;

	case GLFW_KEY_MENU:          return Key::Apps;
	}

	return Key::Undefined;
}


} // namespace GLFW

} // namespace ViewDesign
