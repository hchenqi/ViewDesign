#pragma once

#include "ViewDesign/geometry/point.h"

#include <string>


namespace ViewDesign {

class ViewBase;


struct Ime {
	static void Enable(ViewBase& view);
	static void Disable(ViewBase& view);
	static void SetPosition(ViewBase& view, Point point);

	static std::wstring GetString();
	static size_t GetCursorPosition();
};

constexpr Ime ime;


} // namespace ViewDesign
