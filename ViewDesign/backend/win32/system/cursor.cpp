#include "ViewDesign/system/cursor.h"

#include <windows.h>


namespace ViewDesign {


void SetCursor(Cursor cursor) {
	if (cursor == Cursor::Default) { return; }
	::SetCursor(LoadCursorW(NULL, MAKEINTRESOURCEW(cursor)));
}


} // namespace ViewDesign
