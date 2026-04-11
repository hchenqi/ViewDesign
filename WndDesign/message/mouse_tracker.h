#pragma once

#include "mouse_event.h"
#include "timer.h"
#include "../geometry/helper.h"


namespace ViewDesign {


enum class MouseTrackEvent {
	None,
	LeftDown,
	LeftUp,
	MouseMove,
	LeftClick,			   // down - up (move < 5px is also accepted)
	LeftDoubleClick,       // down - up - down(<0.5s)
	LeftTripleClick,       // down - up - down(<0.5s) - up - down(<0.5s)
	LeftDrag,              // down - move
};


class MouseTracker : Uncopyable {
private:
	uint hit_count = 0;
	static constexpr uint timer_interval = 500;  // 500ms
	Timer timer = Timer([&]() { hit_count = 0; timer.Stop(); });
public:
	bool is_mouse_down = false;
	Point mouse_down_position;
private:
	static constexpr float move_tolerate_range = 5.0f;  // 5px
public:
	MouseTrackEvent Track(MouseEvent event) {
		MouseTrackEvent ret;
		switch (event.type) {
		case MouseEvent::LeftDown:
			switch (hit_count) {
			case 1: ret = MouseTrackEvent::LeftDoubleClick; break;
			case 2: ret = MouseTrackEvent::LeftTripleClick; break;
			case 0: default: ret = MouseTrackEvent::LeftDown; hit_count = 0; break;
			}
			if (square_distance(event.point, mouse_down_position) > square(move_tolerate_range)) {
				ret = MouseTrackEvent::LeftDown; hit_count = 0;
			}
			hit_count++; timer.Set(timer_interval);
			is_mouse_down = true;
			mouse_down_position = event.point;
			break;
		case MouseEvent::LeftUp:
			ret = is_mouse_down ? MouseTrackEvent::LeftClick : MouseTrackEvent::LeftUp;
			is_mouse_down = false;
			break;
		case MouseEvent::Move:
			ret = is_mouse_down ? MouseTrackEvent::LeftDrag : MouseTrackEvent::MouseMove;
			break;
		default:
			ret = MouseTrackEvent::None;
			break;
		}
		return ret;
	}
};


} // namespace ViewDesign
