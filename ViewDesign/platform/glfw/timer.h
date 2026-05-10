#pragma once

#include "ViewDesign/event/timer.h"


namespace ViewDesign {

namespace GLFW {


Handle SetTimer(uint period, Timer& timer); // milliseconds
void ResetTimer(Handle handle, uint period);
void KillTimer(Handle handle);


} // namespace GLFW

} // namespace ViewDesign
