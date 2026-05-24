#pragma once

#include "ViewDesign/event/mouse_event.h"
#include "ViewDesign/event/timer.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


enum class MouseTrackEvent {
	None,
	LeftDown,
	LeftUp,
	MouseMove,
	LeftClick,       // down - up (move distance < 5px)
	LeftDoubleClick, // down - up - down(< 0.5s)
	LeftTripleClick, // down - up - down(< 0.5s) - up - down(< 0.5s)
	LeftDrag,        // down - move
};


class MouseTracker {
public:
	bool down = false;
	Point down_position;
	uint32 hit_count = 0;
private:
	static constexpr uint32 timer_interval = 500; // 0.5s
	Timer timer = Timer([&]() { hit_count = 0; timer.Stop(); });
private:
	static constexpr float move_tolerate_distance = 5.0f; // 5px
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
			if (hit_count > 0 && !PointInCircle(event.point, down_position, move_tolerate_distance)) {
				ret = MouseTrackEvent::LeftDown; hit_count = 0;
			}
			hit_count++; timer.Set(timer_interval);
			down = true;
			down_position = event.point;
			break;
		case MouseEvent::LeftUp:
			ret = down ? MouseTrackEvent::LeftClick : MouseTrackEvent::LeftUp;
			down = false;
			break;
		case MouseEvent::Move:
			ret = down ? MouseTrackEvent::LeftDrag : MouseTrackEvent::MouseMove;
			break;
		default:
			ret = MouseTrackEvent::None;
			break;
		}
		return ret;
	}
};


} // namespace ViewDesign
