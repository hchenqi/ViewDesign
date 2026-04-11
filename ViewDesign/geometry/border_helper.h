#pragma once

#include "geometry.h"
#include "../style/cursor_style.h"


namespace ViewDesign {


enum class BorderPosition : uint {
	None = 0,
	Left = 10,			// HTLEFT
	Top = 12,			// HTTOP
	Right = 11,			// HTRIGHT
	Bottom = 15,		// HTBOTTOM
	LeftTop = 13,		// HTTOPLEFT
	RightTop = 14,		// HTTOPRIGHT
	LeftBottom = 16,	// HTBOTTOMLEFT
	RightBottom = 17	// HTBOTTOMRIGHT
};

inline BorderPosition HitTestBorderPosition(Size window_size, float border_width, Point point) {
	float x1 = border_width, y1 = border_width;
	float x2 = window_size.width - border_width, y2 = window_size.height - border_width;
	float width = window_size.width, height = window_size.height;
	float x = point.x, y = point.y;
	if (x < x1) {
		if (y < y1) {
			return BorderPosition::LeftTop;
		} else if (y < y2) {
			return BorderPosition::Left;
		} else {
			return BorderPosition::LeftBottom;
		}
	} else if (x < x2) {
		if (y < y1) {
			return BorderPosition::Top;
		} else if (y < y2) {
			return BorderPosition::None;
		} else {
			return BorderPosition::Bottom;
		}
	} else {
		if (y < y1) {
			return BorderPosition::RightTop;
		} else if (y < y2) {
			return BorderPosition::Right;
		} else {
			return BorderPosition::RightBottom;
		}
	}
}

inline Cursor GetBorderPositionCursor(BorderPosition border_position) {
	switch (border_position) {
	case BorderPosition::Left: return Cursor::ResizeWE;
	case BorderPosition::Top: return Cursor::ResizeNS;
	case BorderPosition::Right: return Cursor::ResizeWE;
	case BorderPosition::Bottom: return Cursor::ResizeNS;
	case BorderPosition::LeftTop: return Cursor::ResizeNWSE;
	case BorderPosition::RightTop: return Cursor::ResizeNESW;
	case BorderPosition::LeftBottom: return Cursor::ResizeNESW;
	case BorderPosition::RightBottom: return Cursor::ResizeNWSE;
	default: return Cursor::Default;
	}
}


} // namespace ViewDesign
