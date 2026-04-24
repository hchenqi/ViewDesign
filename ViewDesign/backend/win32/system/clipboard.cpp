#include "ViewDesign/platform/win32/clipboard.h"


namespace ViewDesign {


void SetClipboardText(std::wstring text) { Win32::SetClipboardText(std::move(text)); }
	
void GetClipboardText(std::wstring& text) { Win32::GetClipboardText(text); }


} // namespace ViewDesign
