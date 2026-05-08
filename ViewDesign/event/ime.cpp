#include "ViewDesign/event/ime.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct DesktopPrivateAccess : Desktop {
	using Desktop::ImeEnable;
	using Desktop::ImeDisable;
	using Desktop::ImeSetPosition;
};


namespace {

inline DesktopPrivateAccess& GetDesktop() {
	return static_cast<DesktopPrivateAccess&>(desktop.Get());
}

} // namespace


void Ime::Enable(ViewBase& view) { GetDesktop().ImeEnable(view); }

void Ime::Disable(ViewBase& view) { GetDesktop().ImeDisable(view); }

void Ime::SetPosition(ViewBase& view, Point point) { GetDesktop().ImeSetPosition(view, point); }


} // namespace ViewDesign
