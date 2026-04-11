#include "WndDesign/window/Global.h"
#include "WndDesign/frame/DesktopFrame.h"
#include "WndDesign/frame/InnerBorderFrame.h"
#include "WndDesign/frame/PaddingFrame.h"
#include "WndDesign/frame/ClipFrame.h"
#include "WndDesign/frame/MaxFrame.h"
#include "WndDesign/layout/SplitLayout.h"
#include "WndDesign/control/EditBox.h"


using namespace ViewDesign;


class MainFrame : public DesktopFrame {
public:
	using DesktopFrame::DesktopFrame;
private:
	Size size_ref;
private:
	static Point CalculateCenterPosition(Size size, Size size_ref) {
		return Point((size_ref.width - size.width) / 2, (size_ref.height - size.height) / 2);
	}
private:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override {
		return { Size(100, 100), size_ref };
	}
	virtual Rect OnDesktopFrameSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;
		Rect region;
		region.size = UpdateChildSizeRef(child, size_ref);
		region.point = CalculateCenterPosition(region.size, size_ref);
		return region;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		DesktopFrameRegionUpdated(Rect(CalculateCenterPosition(child_size, size_ref), child_size));
	}
};


struct EditBoxStyle : EditBox::Style {
	EditBoxStyle() {
		font.family(L"DengXian").size(20).weight(FontWeight::Bold).color(Color::Orange);
	}
};


struct TextBoxStyle : TextBlockStyle {
	TextBoxStyle() {
		font.family(L"Segoe UI").size(50).color(Color::Lime);
	}
};


int main() {
	global.AddWnd(
		new MainFrame(
			L"SplitLayoutTest",
			new InnerBorderFrame(
				Border(1.0, Color::Orange),
				new SplitLayoutVertical(
					new SplitLayoutHorizontal(
						new PaddingFrame(
							Padding(30px),
							new EditBox(EditBoxStyle(), L"edit here...")
						),
						new ClipFrame<Auto, Assigned, Top>(
							new MaxFrame<Auto, Auto>(
								Size(150px, length_max),
								new TextBox(TextBoxStyle(), L"Hello World!")
							)
						)
					),
					new ClipFrame<Assigned, Auto, Left>(
						new TextBox(TextBoxStyle(), L"Hello World!")
					)
				)
			)
		)
	);
	global.EventLoop();
}
