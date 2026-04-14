#pragma once

#include "../geometry/geometry.h"

#include <memory>


namespace ViewDesign {

class ViewBase;
class Window;


struct Global {
	static Window& AddWindow(std::unique_ptr<Window> window);
	static Window& AddWindow(alloc_ptr<Window> window);
	static std::unique_ptr<Window> RemoveWindow(Window& window);

	static Window& GetWindow(ViewBase& view);
	static Window& GetWindowPoint(ViewBase& view, Point& point);

	static void EventLoop();
	static void Terminate();
};

constexpr Global global;


} // namespace ViewDesign
