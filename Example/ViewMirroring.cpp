#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/MirroringFrame.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/MaxFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/layout/StackLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/wrapper/HitTestHelper.h>

using namespace ViewDesign;

void App() {
	auto mirroring_frame = create<MirroringFrame<Fixed, Fixed>>(
		new ScrollFrame<Vertical>(
			new ClipFrame<Fixed, Auto, Left>(
				new MaxFrame<Relative, Auto>(
					length_infinite,
					new PaddingFrame(
						Padding(10.0f),
						new TextEditor(TextEditor::Style(), u"Type something here...")
					)
				)
			)
		)
	);
	auto& mirroring_frame_ref = *mirroring_frame;

	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"ViewMirroring",
			new StackLayoutMultiple(
				std::move(mirroring_frame),
				create<HitThrough<ClipFrame<Fixed, Fixed, BottomRight>>>(
					new BackgroundFrame(
						Color(Color::LightBlue, 0x80),
						new ScaleFrame(
							Scale(0.5f),
							mirroring_frame_ref.CreateMirrorView()
						)
					)
				),
				create<HitThrough<ClipFrame<Fixed, Fixed, TopRight>>>(
					new BackgroundFrame(
						Color(Color::Fuchsia, 0x40),
						new ScaleFrame(
							Scale(0.3f),
							mirroring_frame_ref.CreateMirrorView()
						)
					)
				)
			)
		)
	);
	desktop.EventLoop();
}
