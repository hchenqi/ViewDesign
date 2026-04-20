#include "ViewDesign/event/ime.h"
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/backend/win32/ime.h"


namespace ViewDesign {

struct DesktopApi : Desktop {
	Desktop::ImeEnable;
	Desktop::ImeDisable;
	Desktop::ImeSetPosition;
};


void Ime::Enable(ViewBase& view) { static_cast<DesktopApi&>(desktop).ImeEnable(view); }

void Ime::Disable(ViewBase& view) { static_cast<DesktopApi&>(desktop).ImeDisable(view); }

void Ime::SetPosition(ViewBase& view, Point point) { static_cast<DesktopApi&>(desktop).ImeSetPosition(view, point); }

std::wstring Ime::GetString() { return ImeGetString(); }

size_t Ime::GetCursorPosition() { return ImeGetCursorPosition(); }


} // namespace ViewDesign
