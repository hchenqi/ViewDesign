#include "WndDesign/window/Global.h"
#include "WndDesign/frame/DesktopFrame.h"
#include "WndDesign/frame/ClipFrame.h"
#include "WndDesign/frame/InnerBorderFrame.h"
#include "WndDesign/frame/ScaleFrame.h"
#include "WndDesign/control/TextBox.h"
#include "WndDesign/style/border_style.h"
#include "WndDesign/geometry/helper.h"
#include "WndDesign/system/cursor.h"
#include "WndDesign/system/win32_aero_snap.h"

#include <cmath>


using namespace ViewDesign;


class MainFrame : public DesktopFrame {
public:
	using DesktopFrame::DesktopFrame;
private:
	Size size = Size(800, 500);
private:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) {
		return { Size(100, 100), size_ref };
	}
	virtual Rect OnDesktopFrameSizeRefUpdate(Size size_ref) override {
		Rect region;
		region.size = UpdateChildSizeRef(child, size);
		region.point.x = (size_ref.width - region.size.width) / 2;
		region.point.y = (size_ref.height - region.size.height) / 2;
		return region;
	}
};


class ScaleLayer : public ScaleFrame<Assigned, Assigned> {
public:
	ScaleLayer(child_type child) : ScaleFrame(Scale(1.0), std::move(child)) {}
private:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		if (event.ctrl && event.type == MouseEvent::WheelVertical) {
			return this;
		}
		return ScaleFrame::HitTest(event);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		SetScale(scale * Scale(powf(1.1f, event.wheel_delta / 120.0f)));
	}
};


class ResizeBorder : public InnerBorderFrame<Assigned, Assigned> {
public:
	ResizeBorder(child_type child) : InnerBorderFrame<Assigned, Assigned>(Border(5.0, Color(Color::Yellow, 127)), std::move(child)) {}
private:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		if (PointInRoundedRectangle(event.point, Extend(Rect(point_zero, size), -border._width), border._radius)) { return child; }
		return this;
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		if (event.type == MouseEvent::Move || event.type == MouseEvent::LeftDown) {
			BorderPosition border_position = HitTestBorderPosition(size, border._width + border._radius, event.point);
			if (event.type == MouseEvent::Move) {
				SetCursor(GetBorderPositionCursor(border_position));
			} else {
				AeroSnapBorderResizingEffect(*this, border_position);
			}
		}
	}
};


struct TextBoxStyle : TextBlockStyle {
	TextBoxStyle() {
		font.family(L"Segoe UI").size(75).color(Color::Lime);
	}
};


int main() {
	global.AddWnd(
		new MainFrame(
			L"TextBoxTest",
			new ScaleLayer(
				new ResizeBorder(
					new ClipFrame<Assigned, Assigned, TopLeft>(
						new InnerBorderFrame(
							Border(1.0, Color::Black),
							new TextBox(TextBoxStyle(), L"Hello World!")
						)
					)
				)
			)
		)
	);
	global.EventLoop();
}
