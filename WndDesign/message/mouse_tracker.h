#pragma once

#include "mouse_msg.h"
#include "timer.h"
#include "../geometry/helper.h"


namespace ViewDesign {


enum class MouseTrackMsg {
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
	MouseTrackMsg Track(MouseMsg msg) {
		MouseTrackMsg ret;
		switch (msg.type) {
		case MouseMsg::LeftDown:
			switch (hit_count) {
			case 1: ret = MouseTrackMsg::LeftDoubleClick; break;
			case 2: ret = MouseTrackMsg::LeftTripleClick; break;
			case 0: default: ret = MouseTrackMsg::LeftDown; hit_count = 0; break;
			}
			if (square_distance(msg.point, mouse_down_position) > square(move_tolerate_range)) {
				ret = MouseTrackMsg::LeftDown; hit_count = 0;
			}
			hit_count++; timer.Set(timer_interval);
			is_mouse_down = true;
			mouse_down_position = msg.point;
			break;
		case MouseMsg::LeftUp:
			ret = is_mouse_down ? MouseTrackMsg::LeftClick : MouseTrackMsg::LeftUp;
			is_mouse_down = false;
			break;
		case MouseMsg::Move:
			ret = is_mouse_down ? MouseTrackMsg::LeftDrag : MouseTrackMsg::MouseMove;
			break;
		default:
			ret = MouseTrackMsg::None;
			break;
		}
		return ret;
	}
};


} // namespace ViewDesign
