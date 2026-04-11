#pragma once

#include "Global.h"
#include "Desktop.h"
#include "DesktopFrame.h"


namespace ViewDesign {


DesktopFrame& Global::Add(std::unique_ptr<DesktopFrame> frame) { return desktop.AddChild(std::move(frame)); }
DesktopFrame& Global::Add(alloc_ptr<DesktopFrame> frame) { return Add(std::unique_ptr<DesktopFrame>(frame)); }
std::unique_ptr<DesktopFrame> Global::Remove(DesktopFrame& frame) { return desktop.RemoveChild(frame); }

DesktopFrame& Global::GetDesktopFrame(ViewBase& view) { return desktop.GetDesktopFrame(view); }
DesktopFrame& Global::GetDesktopFramePoint(ViewBase& view, Point& point) { return desktop.GetDesktopFramePoint(view, point); }

void Global::EventLoop() { desktop.EventLoop(); }
void Global::Terminate() { desktop.Terminate(); }


} // namespace ViewDesign
