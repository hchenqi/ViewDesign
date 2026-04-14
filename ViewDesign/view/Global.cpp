#pragma once

#include "Global.h"
#include "Desktop.h"
#include "Window.h"


namespace ViewDesign {


Window& Global::AddWindow(std::unique_ptr<Window> window) { return desktop.AddWindow(std::move(window)); }
Window& Global::AddWindow(alloc_ptr<Window> window) { return AddWindow(std::unique_ptr<Window>(window)); }
std::unique_ptr<Window> Global::RemoveWindow(Window& window) { return desktop.RemoveWindow(window); }

Window& Global::GetWindow(ViewBase& view) { return desktop.GetWindow(view); }
Window& Global::GetWindowPoint(ViewBase& view, Point& point) { return desktop.GetWindowPoint(view, point); }

void Global::EventLoop() { desktop.EventLoop(); }
void Global::Terminate() { desktop.Terminate(); }


} // namespace ViewDesign
