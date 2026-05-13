#pragma once

#include "ViewDesign/event/timer.h"


namespace ViewDesign {

namespace GLFW {


Handle SetTimer(uint32 period, Timer& timer); // milliseconds
void ResetTimer(Handle handle, uint32 period);
void KillTimer(Handle handle);


} // namespace GLFW

} // namespace ViewDesign
