#include "ViewDesign/platform/win32/clipboard.h"
#include "ViewDesign/platform/win32/string.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void SetClipboardText(u16string text) {
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL hdata = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (text.length() + 1) * sizeof(u16char));
		if (hdata != nullptr) {
			u16char* data = (u16char*)GlobalLock(hdata);
			if (data != nullptr) { wcscpy_s(as_wchar_str(data), text.length() + 1, as_wchar_str(text.data())); }
			GlobalUnlock(hdata);
			if (!::SetClipboardData(CF_UNICODETEXT, hdata)) { GlobalFree(hdata); }
		}
		CloseClipboard();
	}
}

void GetClipboardText(u16string& text) {
	if (OpenClipboard(NULL)) {
		HANDLE hdata = ::GetClipboardData(CF_UNICODETEXT);
		if (hdata != nullptr) {
			const u16char* data = (const u16char*)GlobalLock(hdata);
			if (data != nullptr) { text.assign(data); }
			GlobalUnlock(hdata);
		}
		CloseClipboard();
	}
}


} // namespace Win32

} // namespace ViewDesign
