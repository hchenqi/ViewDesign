#pragma once

#include "key_event.h"


namespace ViewDesign {


class KeyTracker {
public:
	bool ctrl = false;
	bool shift = false;
public:
	void Track(KeyEvent event) {
		switch (event.type) {
		case KeyEvent::KeyDown:
			switch (event.key) {
			case Key::Ctrl: ctrl = true; break;
			case Key::Shift: shift = true; break;
			}
			break;
		case KeyEvent::KeyUp:
			switch (event.key) {
			case Key::Ctrl: ctrl = false; break;
			case Key::Shift: shift = false; break;
			}
			break;
		}
	}
};


} // namespace ViewDesign
