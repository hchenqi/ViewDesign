#include "ViewDesign/event/ime.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct DesktopApi : Desktop {
	using Desktop::ImeEnable;
	using Desktop::ImeDisable;
	using Desktop::ImeSetPosition;
};


void Ime::Enable(ViewBase& view) { static_cast<DesktopApi&>(desktop).ImeEnable(view); }

void Ime::Disable(ViewBase& view) { static_cast<DesktopApi&>(desktop).ImeDisable(view); }

void Ime::SetPosition(ViewBase& view, Point point) { static_cast<DesktopApi&>(desktop).ImeSetPosition(view, point); }


} // namespace ViewDesign
