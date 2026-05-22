#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/frame/BackgroundFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/control/EditBox.h"

#include <windows.h>


using namespace ViewDesign;


// hot reload requires the states to be stored out of the components and the components access the states by reference, so that the state won't be lost when the components are recreated
// EditBox in the default component library of ViewDesign owns its text string internally, so here EditBoxRef extends EditBox to accept an external text string by reference and update it whenever the text is modified
// alternatively, one can write their own versions of TextBox/EditBox that just accept a reference to an external text string

class EditBoxRef : public EditBox {
public:
	using Style = EditBox::Style;
public:
	EditBoxRef(const Style& style, u16string& text_ref) : EditBox(style, text_ref), text_ref(text_ref) {}
private:
	u16string& text_ref;
private:
	virtual void OnTextUpdate() override {
		text_ref = text;
		EditBox::OnTextUpdate();
	}
};


// hint: set code_version to a different value with each edit
static constexpr int code_version = 0;


struct EditBoxStyle : EditBoxRef::Style {
	EditBoxStyle() {
		// hint: edit the styles while running
		font.size(36.0f).color(Color::ForestGreen);
	}
};


void App() {
	// while a normal ViewFrame doesn't support resetting its child view, MutableFrame extends ViewFrame and allows the child view to be reset
	// with MutableFrame we can recreate its child view when hot reload happens
	ref_ptr<MutableFrame<Fixed, Fixed>> mutable_frame;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Hot Reload",
			mutable_frame = new MutableFrame(
				new Placeholder<Fixed, Fixed>()
			)
		)
	);
	// the pointer mutable_frame remains valid until the window is closed

	int current_code_version = -1;
	u16string text = u"Type something here...";

	// the default desktop.EventLoop() is blocking
	// here we define the message loop ourselves
	for (;;) {
		for (MSG msg; PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		if (current_code_version != code_version) {
			current_code_version = code_version;

			// hint: edit the styles while running
			// hot reload for adding/removing frames or changing frame template arguments might not be supported by Visual Studio
			// this is because the frame templates won't be instantiated until they are referenced and instantiation introduces new data types
			// we could explicitly instantiate the templates that might be used so that their symbols stay valid (see the end of this file)
			mutable_frame->Reset(
				new BackgroundFrame(
					Color::PaleTurquoise,
					new ClipFrame<Fixed, Fixed, TopLeft>(
						new PaddingFrame(
							Padding(15.0f),
							new EditBoxRef(EditBoxStyle(), text)
						)
					)
				)
			);
		}

		if (desktop.WindowListEmpty()) {
			break;
		}
	}
}


#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// explicitly instantiate and export the templates that might be used to force the linker to keep their symbols
template class EXPORT ClipFrame<Fixed, Fixed, TopLeft>;
template class EXPORT ClipFrame<Fixed, Fixed, TopRight>;
template class EXPORT ClipFrame<Fixed, Fixed, BottomLeft>;
template class EXPORT ClipFrame<Fixed, Fixed, BottomRight>;
