#include "ime.h"
#include "../view/Desktop.h"
#include "../system/win32_ime.h"


namespace ViewDesign {


void Ime::Enable(ViewBase& view) { desktop.ImeEnable(view); }

void Ime::Disable(ViewBase& view) { desktop.ImeDisable(view); }

void Ime::SetPosition(ViewBase& view, Point point) { desktop.ImeSetPosition(view, point); }

std::wstring Ime::GetString() { return ImeGetString(); }

size_t Ime::GetCursorPosition() { return ImeGetCursorPosition(); }


} // namespace ViewDesign
