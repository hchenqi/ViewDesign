#pragma once

#include "ViewDesign/event/mouse_event.h"
#include "ViewDesign/geometry/helper.h"

#include <chrono>


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
	static constexpr float move_tolerate_distance{ 5.0f }; // 5px
	static constexpr std::chrono::milliseconds hit_timeout{ 500 }; // 500ms
public:
	bool down = false;
	Point down_position;
	uint32 hit_count = 0;
	std::chrono::steady_clock::time_point last_hit_time;
public:
	MouseTrackEvent Track(MouseEvent event) {
		switch (event.type) {
		case MouseEvent::LeftDown:
			if (hit_count > 0 && (hit_count >= 3 || !PointInCircle(event.point, down_position, move_tolerate_distance) || std::chrono::steady_clock::now() - last_hit_time > hit_timeout)) {
				hit_count = 0;
			}
			down = true;
			down_position = event.point;
			hit_count++;
			last_hit_time = std::chrono::steady_clock::now();
			switch (hit_count) {
			case 1: return MouseTrackEvent::LeftDown;
			case 2: return MouseTrackEvent::LeftDoubleClick;
			case 3: return MouseTrackEvent::LeftTripleClick;
			}
		case MouseEvent::LeftUp:
			return std::exchange(down, false) ? MouseTrackEvent::LeftClick : MouseTrackEvent::LeftUp;
		case MouseEvent::Move:
			return down ? MouseTrackEvent::LeftDrag : MouseTrackEvent::MouseMove;
		default:
			return MouseTrackEvent::None;
		}
	}
};


} // namespace ViewDesign
