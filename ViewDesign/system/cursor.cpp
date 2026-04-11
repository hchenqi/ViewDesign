#include "cursor.h"

#include <Windows.h>


namespace ViewDesign {


void SetCursor(Cursor cursor) {
	if (cursor == Cursor::Default) { return; }
	::SetCursor(LoadCursorW(NULL, MAKEINTRESOURCE(cursor)));
}


} // namespace ViewDesign
