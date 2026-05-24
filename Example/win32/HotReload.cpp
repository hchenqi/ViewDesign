// a demonstration of hot reload (immediate mode)
// works with Visual Studio Hot Reload for C++ (https://learn.microsoft.com/en-us/visualstudio/debugger/edit-and-continue-visual-cpp?view=visualstudio)
// the compile option /ZI and link option /INCREMENTAL for this file are already set in CMakeLists.txt

#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/frame/BackgroundFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/control/EditBox.h"

#include <windows.h>


// hot reload (immediate mode) requires the states to be stored externally and the components access and update the states by reference, so that the state won't be lost when the components are recreated
// EditBox in the standard component library of ViewDesign owns its states internally, therefore, it is extended here to accept an external state reference which is synchronized with its internal states on events
// alternatively, one can write their own versions of EditBox that just reference an external state without owning internal states
// other stateless components like TextBox, ClipFrame, BorderFrame, BackgroundFrame, etc can be used directly in immediate mode

namespace ViewDesign::IM {

class EditBox : public ViewDesign::EditBox {
public:
	using Style = ViewDesign::EditBox::Style;
public:
	struct State {
		// some states are not initialized here just because their values should initially be undefined
		u16string text = u"";
		CaretState caret_state = CaretState::Hide;
		TextRange caret_position;
		TextRange selection_range = text_range_empty;
		SelectionMode selection_mode;
		TextRange selection_initial_range;
		TextRange ime_composition_range = text_range_empty;
		bool mouse_down = false;
		Point mouse_down_position;
		uint32 mouse_hit_count = 0;
		bool key_ctrl = false;
		bool key_shift = false;
		bool focus = false;
	};
public:
	EditBox(const Style& style, State& state) : ViewDesign::EditBox(style, state.text), state(state) {}

private:
	State& state;
public:
	void SaveState() {
		// state.text = text; // saved at OnTextUpdate 
		state.caret_state = caret_state;
		state.caret_position = caret_position;
		state.selection_range = selection_range;
		state.selection_mode = selection_mode;
		state.selection_initial_range = selection_initial_range;
		state.ime_composition_range = ime_composition_range;
		state.mouse_down = mouse_tracker.down;
		state.mouse_down_position = mouse_tracker.down_position;
		state.mouse_hit_count = mouse_tracker.hit_count;
		state.key_ctrl = key_tracker.ctrl;
		state.key_shift = key_tracker.shift;
		// state.focus updated at OnFocusEvent
	}
	void LoadState() {
		//Assign(state.text); // loaded at construction
		caret_state = state.caret_state;
		if (caret_state != CaretState::Hide) { UpdateCaret(state.caret_position); }
		UpdateSelection(state.selection_range);
		selection_mode = state.selection_mode;
		selection_initial_range = state.selection_initial_range;
		UpdateImeComposition(state.ime_composition_range);
		mouse_tracker.down = state.mouse_down;
		mouse_tracker.down_position = state.mouse_down_position;
		mouse_tracker.hit_count = state.mouse_hit_count;
		key_tracker.ctrl = state.key_ctrl;
		key_tracker.shift = state.key_shift;
		if (state.focus) { SetFocus(); }
		if (state.mouse_down) { SetCapture(); }
	}
private:
	virtual void OnTextUpdate() override {
		ViewDesign::EditBox::OnTextUpdate();
		state.text = text;
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		ViewDesign::EditBox::OnMouseEvent(event);
		SaveState();
	}
	virtual void OnKeyEvent(KeyEvent event) override {
		ViewDesign::EditBox::OnKeyEvent(event);
		SaveState();
	}
	virtual void OnFocusEvent(FocusEvent event) override {
		ViewDesign::EditBox::OnFocusEvent(event);
		switch (event) {
		case FocusEvent::Focus: state.focus = true; break;
		case FocusEvent::Blur: state.focus = false; break;
		}
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

	IM::EditBox::State edit_box_state;
	edit_box_state.text = u"Type something here...";

	int current_version = -1;

	// the default desktop.EventLoop() is blocking
	// here we define the Win32 message loop ourselves
	for (;;) {
		for (MSG msg; PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

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
		struct EditBoxStyle : IM::EditBox::Style {
			EditBoxStyle() {
				font.size(36.0f).color(Color::ForestGreen);
			}
		};

		// hot reload for adding/removing frame templates or changing template arguments is not supported by Visual Studio
		// this is because instantiation of templates introduces new data types
		// we could explicitly instantiate the templates that might be used so that their symbols stay static (see the end of this file)
		ref_ptr<IM::EditBox> edit_box;
		mutable_frame->Reset(
			new BackgroundFrame(
				Color::PaleTurquoise,
				new ClipFrame<Fixed, Fixed, TopLeft>(
					new PaddingFrame(
						Padding(15.0f),
						edit_box = new IM::EditBox(EditBoxStyle(), edit_box_state)
					)
				)
			)
		);
		// EditBoxRef is detached from the view tree when created and SetFocus() / SetCapture() won't work until it is added in the view tree
		edit_box->LoadState();

		// the caret blinking state, mouse tracker state and cursor might not display consistently without LAZY_RECREATE, because:
		// - caret blinking and mouse tracker uses internal timers that are not set at LoadState
		// - the cursor resets when the tracked view is destructed but the newly added view won't automatically be tracked by Desktop until the next mouse event
		// however, mouse capture and key focus states can be saved and restored through the newly added view itself
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
