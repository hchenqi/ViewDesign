#pragma once

#include "../geometry/point.h"

#include <string>


namespace ViewDesign {

class WndObject;


struct Ime {
	static void Enable(WndObject& wnd);
	static void Disable(WndObject& wnd);
	static void SetPosition(WndObject& wnd, Point point);

	static std::wstring GetString();
	static size_t GetCursorPosition();
};

constexpr Ime ime;


} // namespace ViewDesign
