#pragma once

#include "../geometry/geometry.h"

#include <memory>


namespace ViewDesign {

class ViewBase;
class DesktopFrame;


struct Global {
	static DesktopFrame& Add(std::unique_ptr<DesktopFrame> frame);
	static DesktopFrame& Add(alloc_ptr<DesktopFrame> frame);
	static std::unique_ptr<DesktopFrame> Remove(DesktopFrame& frame);

	static DesktopFrame& GetDesktopFrame(ViewBase& view);
	static DesktopFrame& GetDesktopFramePoint(ViewBase& view, Point& point);

	static void EventLoop();
	static void Terminate();
};

constexpr Global global;


} // namespace ViewDesign
