#include "ViewDesign/view/Global.h"
#include "ViewDesign/view/DesktopFrame.h"
#include "ViewDesign/view/frame/InnerBorderFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/control/EditBox.h"


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
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
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
	global.Add(
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
