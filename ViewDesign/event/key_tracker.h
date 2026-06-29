#pragma once

#include "ViewDesign/event/key_event.h"


namespace ViewDesign {

namespace Stateful {


class KeyTracker {
public:
	struct State {
		bool ctrl = false;
		bool shift = false;
	};
public:
	KeyTracker(State& state) : state(state) {}
protected:
	State& state;
public:
	void Track(KeyEvent event) {
		switch (event.type) {
		case KeyEvent::KeyDown:
			switch (event.key) {
			case Key::Ctrl: case Key::LCtrl: case Key::RCtrl: state.ctrl = true; break;
			case Key::Shift: case Key::LShift: case Key::RShift: state.shift = true; break;
			}
			break;
		case KeyEvent::KeyUp:
			switch (event.key) {
			case Key::Ctrl: case Key::LCtrl: case Key::RCtrl: state.ctrl = false; break;
			case Key::Shift: case Key::LShift: case Key::RShift: state.shift = false; break;
			}
			break;
		}
	}
};


} // namespace Stateful


class KeyTracker : public Stateful::KeyTracker::State, public Stateful::KeyTracker {
public:
	KeyTracker() : Stateful::KeyTracker(static_cast<State&>(*this)) {}
};


} // namespace ViewDesign
