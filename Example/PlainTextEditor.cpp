#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/MaxFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

void App() {
	ref_ptr<TextEditor> text_editor;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"PlainTextEditor",
			new DefaultBackground<ClipFrame<Fixed, Fixed, Top>>(
				new ScrollFrame<Fixed, Bounded>(
					new ClipFrame<Fixed, Auto, Left>(
						new MaxFrame<Bounded, Auto>(
							length_infinite,
							// this padding is not scaled
							new PaddingFrame(
								Padding(10.0f),
								new ScaleFrame(
									// this padding is scaled
									new PaddingFrame(
										Padding(10.0f),
										text_editor = new WithHistory<TextEditor>(TextEditor::Style(), u"Type something here...\n(hint: zoom with Ctrl + scroll)")
									)
								)
							)
						)
					)
				)
			)
		)
	);
	text_editor->Edit(22); // length of u"Type something here..."
	desktop.EventLoop();
}
