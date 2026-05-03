#include "ViewDesign/platform/glfw/timer.h"


namespace ViewDesign {

using namespace GLFW;


void Timer::Set(uint period) {
	if (!IsSet()) {
		timer = SetTimer(period, *this);
	} else {
		ResetTimer(timer, period);
	}
}

void Timer::Stop() {
	if (!IsSet()) { return; }
	KillTimer(timer);
	timer = nullptr;
}


} // namespace ViewDesign
