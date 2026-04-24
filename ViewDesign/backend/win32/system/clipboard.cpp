#include "ViewDesign/platform/win32/clipboard.h"


namespace ViewDesign {


void SetClipboardText(u16string text) { Win32::SetClipboardText(std::move(text)); }
	
void GetClipboardText(u16string& text) { Win32::GetClipboardText(text); }


} // namespace ViewDesign
