// a demonstration of hot reload (immediate mode)
// works with Visual Studio Hot Reload for C++ (https://learn.microsoft.com/en-us/visualstudio/debugger/edit-and-continue-visual-cpp?view=visualstudio)
// the compile option /ZI and link option /INCREMENTAL for this file are already set in CMakeLists.txt

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/MutableFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/control/TextEditor.h>


// hot reload (immediate mode) requires the states to be stored externally
// the components access and update the states by reference, so that the states won't be lost when the components are recreated
// we use Stateful::TextEditor in the standard component library of ViewDesign

namespace ViewDesign::IM {

class TextEditor : public Stateful::TextEditor {
public:
	TextEditor(Style style, State& state) : Stateful::TextEditor(std::move(style), state) {}
public:
	void RestoreFocusCapture() {
		if (state.caret.active) { SetFocus(); }
		if (state.input.mouse.down) { SetCapture(); }
	}
};

} // namespace ViewDesign::IM


using namespace ViewDesign;


void App() {
	// while a normal ViewFrame doesn't support resetting its child view, MutableFrame extends ViewFrame and allows its child view to be reset
	// MutableFrame re-computes the layout and redraws when its child view is reset
	ref_ptr<MutableFrame<Fixed, Fixed>> mutable_frame;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Hot Reload (Immediate Mode)",
			// save the mutable_frame pointer here just as a reference
			// it remains valid until the window is closed
			mutable_frame = new MutableFrame(
				new Placeholder<Fixed, Fixed>()
			)
		)
	);

	IM::TextEditor::State text_editor_state;
	text_editor_state.text = u"Type something here...";

	int current_version = -1;

	for (;;) {
		desktop.PollEvents();

		if (desktop.WindowListEmpty()) {
			break;
		}

		// enable this macro to lazily recreate components only when the code version changes
//#define LAZY_RECREATE

#if defined(LAZY_RECREATE)
		// hint: modify version to a different value with each edit
		static constexpr int version = 0;
		if (current_version == version) {
			continue;
		} else {
			current_version = version;
		}
#endif

		// hint: edit the styles while running
		struct TextEditorStyle : IM::TextEditor::Style {
			TextEditorStyle() {
				font.size(36.0f).color(ColorCode::ForestGreen);
			}
		};

		// hot reload for adding/removing frame templates or changing template arguments is not supported by Visual Studio
		// this is because instantiation of templates introduces new data types
		// we could explicitly instantiate the templates that might be used so that their symbols stay static (see the end of this file)
		ref_ptr<IM::TextEditor> text_editor;
		mutable_frame->Reset(
			new BackgroundFrame(
				ColorCode::PaleTurquoise,
				new ClipFrame<Fixed, Fixed, TopLeft>(
					new PaddingFrame(
						Padding(15.0f),
						text_editor = new IM::TextEditor(TextEditorStyle(), text_editor_state)
					)
				)
			)
		);
		// TextEditor is detached from the view tree when created and SetFocus() / SetCapture() won't work until it is added in the view tree
		text_editor->RestoreFocusCapture();

		// the cursor might not display consistently without LAZY_RECREATE
		// because the cursor resets when the tracked view is destructed but the newly added view won't automatically be tracked by Desktop until the next mouse event
	}
}


#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

namespace ViewDesign {

// explicitly instantiate and export the templates that might be used to force the linker to keep their symbols
template class EXPORT ClipFrame<Fixed, Fixed, TopLeft>;
template class EXPORT ClipFrame<Fixed, Fixed, TopRight>;
template class EXPORT ClipFrame<Fixed, Fixed, BottomLeft>;
template class EXPORT ClipFrame<Fixed, Fixed, BottomRight>;

} // ViewDesign
