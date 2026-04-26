#include "ViewDesign/system/cursor.h"
#include "ViewDesign/platform/win32/cursor.h"

#include <array>
#include <vector>


namespace ViewDesign {


class Cursor {
public:
	enum class Type {
		Hidden,
		NoChange,
		Shared,
		Custom,
	};
public:
	Cursor(Type type, HCURSOR handle) : type(type), handle(handle) {}
	~Cursor() {
		if (type == Type::Custom) {
			DestroyCursor(handle);
		}
	}
public:
	Type type;
	HCURSOR handle;
};


namespace {

std::array<Cursor, static_cast<size_t>(CursorStyle::_Count)> cursor_cache_shared = {
	Cursor(Cursor::Type::Hidden, NULL),
	Cursor(Cursor::Type::NoChange, NULL),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_ARROW)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_HAND)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_IBEAM)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_SIZEALL)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_HELP)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_CROSS)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_WAIT)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_APPSTARTING)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_NO)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_SIZEWE)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_SIZENESW)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_SIZENS)),
	Cursor(Cursor::Type::Shared, LoadCursorW(NULL, IDC_SIZENWSE))
};

std::vector<Cursor> cursor_cache_custom;

} // namespace


std::reference_wrapper<Cursor> GetCursor(CursorStyle style) {
	if (style >= CursorStyle::_Count) {
		throw std::out_of_range("GetCursor: invalid cursor style");
	}
	return cursor_cache_shared[static_cast<size_t>(style)];
}

std::reference_wrapper<Cursor> CreateCursor(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot) {
	return cursor_cache_custom.emplace_back(Cursor::Type::Custom, Win32::CreateCursorFromPixelBuffer(pixel_buffer, hotspot));
}


void SetCursor(std::reference_wrapper<Cursor> cursor) {
	switch (cursor.get().type) {
	case Cursor::Type::Hidden:
		::SetCursor(NULL);
		break;
	case Cursor::Type::NoChange:
		break;
	case Cursor::Type::Shared:
	case Cursor::Type::Custom:
		::SetCursor(cursor.get().handle);
		break;
	}
}


Point GetCursorPosition() {
	POINT cursor_position; GetCursorPos(&cursor_position);
	return Point((float)cursor_position.x, (float)cursor_position.y);
}


} // namespace ViewDesign
