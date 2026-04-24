#include "ViewDesign/platform/win32/clipboard.h"
#include "ViewDesign/common/type.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void SetClipboardText(std::wstring text) {
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL hdata = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (text.length() + 1) * sizeof(wchar));
		if (hdata != nullptr) {
			wchar* data = (wchar*)GlobalLock(hdata);
			if (data != nullptr) { wcscpy_s(data, text.length() + 1, text.data()); }
			GlobalUnlock(hdata);
			if (!::SetClipboardData(CF_UNICODETEXT, hdata)) { GlobalFree(hdata); }
		}
		CloseClipboard();
	}
}

void GetClipboardText(std::wstring& text) {
	if (OpenClipboard(NULL)) {
		HANDLE hdata = ::GetClipboardData(CF_UNICODETEXT);
		if (hdata != nullptr) {
			const wchar* data = (const wchar*)GlobalLock(hdata);
			if (data != nullptr) { text.assign(data); }
			GlobalUnlock(hdata);
		}
		CloseClipboard();
	}
}


} // namespace Win32

} // namespace ViewDesign
