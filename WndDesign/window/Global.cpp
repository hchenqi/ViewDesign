#pragma once

#include "Global.h"
#include "desktop.h"
#include "../frame/DesktopFrame.h"


namespace ViewDesign {


DesktopFrame& Global::AddWnd(std::unique_ptr<DesktopFrame> frame) { return desktop.AddChild(std::move(frame)); }
DesktopFrame& Global::AddWnd(alloc_ptr<DesktopFrame> frame) { return AddWnd(std::unique_ptr<DesktopFrame>(frame)); }
std::unique_ptr<DesktopFrame> Global::RemoveWnd(DesktopFrame& frame) { return desktop.RemoveChild(frame); }

DesktopFrame& Global::GetDesktopFrame(WndObject& wnd) { return desktop.GetDesktopFrame(wnd); }
DesktopFrame& Global::GetDesktopFramePoint(WndObject& wnd, Point& point) { return desktop.GetDesktopFramePoint(wnd, point); }

void Global::EventLoop() { desktop.EventLoop(); }
void Global::Terminate() { desktop.Terminate(); }


} // namespace ViewDesign
