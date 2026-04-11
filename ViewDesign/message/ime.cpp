#include "ime.h"
#include "../window/desktop.h"
#include "../system/win32_ime.h"


namespace ViewDesign {


void Ime::Enable(WndObject& wnd) { desktop.ImeEnable(wnd); }

void Ime::Disable(WndObject& wnd) { desktop.ImeDisable(wnd); }

void Ime::SetPosition(WndObject& wnd, Point point) { desktop.ImeSetPosition(wnd, point); }

std::wstring Ime::GetString() { return ImeGetString(); }

size_t Ime::GetCursorPosition() { return ImeGetCursorPosition(); }


} // namespace ViewDesign
