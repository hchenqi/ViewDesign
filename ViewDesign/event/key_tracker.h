#pragma once

#include "ViewDesign/event/key_event.h"


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
			case Key::Ctrl: case Key::LCtrl: case Key::RCtrl: ctrl = true; break;
			case Key::Shift: case Key::LShift: case Key::RShift: shift = true; break;
			}
			break;
		case KeyEvent::KeyUp:
			switch (event.key) {
			case Key::Ctrl: case Key::LCtrl: case Key::RCtrl: ctrl = false; break;
			case Key::Shift: case Key::LShift: case Key::RShift: shift = false; break;
			}
			break;
		}
	}
};


} // namespace ViewDesign
