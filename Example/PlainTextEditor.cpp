#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/control/EditBox.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

void App() {
	ref_ptr<EditBox> edit_box;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"PlainTextEditor",
			new DefaultBackground<ScrollFrame<Vertical>>(
				new ClipFrame<Fixed, Relative, Left>(
					// this padding is not scaled
					new PaddingFrame(
						Padding(10.0f),
						new ScaleFrame(
							// this padding is scaled
							new PaddingFrame(
								Padding(10.0f),
								edit_box = new EditBox(EditBox::Style(), u"Type something here...\n(hint: zoom with Ctrl + scroll)")
							)
						)
					)
				)
			)
		)
	);
	edit_box->Edit(22); // length of u"Type something here..."
	desktop.EventLoop();
}
