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


using namespace ViewDesign;


// hot reload (and immediate mode) requires the states to be stored externally and the components access and update the states by reference, so that the state won't be lost when the components are recreated
// EditBox in the standard component library of ViewDesign owns its states internally, therefore, it is extended here to accept an external state reference which is synchronized with its internal states on events
// alternatively, one can write their own versions of EditBox that just reference an external state without owning internal states
// other stateless components like TextBox, ClipFrame, BorderFrame, BackgroundFrame, etc can be used directly in immediate mode

class EditBoxRef : public EditBox {
public:
	struct State {
		u16string text = u"Type something here...";
		TextRange caret_position = text_range_empty;
		CaretState caret_state = CaretState::Hide;
		SelectionMode selection_mode;
		TextRange selection_initial_range = text_range_empty;
		TextRange selection_range = text_range_empty;
		TextRange ime_composition_range = text_range_empty;
		bool is_mouse_down = false;
		Point mouse_down_position;
		uint32 hit_count = 0;
		bool ctrl = false;
		bool shift = false;
		bool focus = false;
	};

	using Style = EditBox::Style;

public:
	EditBoxRef(const Style& style, State& state) : EditBox(style, state.text), state(state) {}

private:
	State& state;
public:
	void SaveState() {
		// state.text = text; // saved at OnTextUpdate 
		state.caret_position = caret_position;
		state.caret_state = caret_state;
		state.selection_mode = selection_mode;
		state.selection_initial_range = selection_initial_range;
		state.selection_range = selection_range;
		state.ime_composition_range = ime_composition_range;
		state.is_mouse_down = mouse_tracker.is_mouse_down;
		state.mouse_down_position = mouse_tracker.mouse_down_position;
		state.hit_count = mouse_tracker.hit_count;
		state.ctrl = key_tracker.ctrl;
		state.shift = key_tracker.shift;
		// state.focus updated at OnFocusEvent
	}
	void LoadState() {
		//Assign(state.text); // loaded at construction
		UpdateCaret(state.caret_position);
		caret_state = state.caret_state;
		selection_mode = state.selection_mode;
		selection_initial_range = state.selection_initial_range;
		UpdateSelection(state.selection_range);
		UpdateImeComposition(state.ime_composition_range);
		mouse_tracker.is_mouse_down = state.is_mouse_down;
		mouse_tracker.mouse_down_position = state.mouse_down_position;
		mouse_tracker.hit_count = state.hit_count;
		key_tracker.ctrl = state.ctrl;
		key_tracker.shift = state.shift;
		if (state.focus) { SetFocus(); }
		if (state.is_mouse_down) { SetCapture(); }
	}
private:
	virtual void OnTextUpdate() override {
		EditBox::OnTextUpdate();
		state.text = text;
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		EditBox::OnMouseEvent(event);
		SaveState();
	}
	virtual void OnKeyEvent(KeyEvent event) override {
		EditBox::OnKeyEvent(event);
		SaveState();
	}
	virtual void OnFocusEvent(FocusEvent event) override {
		EditBox::OnFocusEvent(event);
		switch (event) {
		case FocusEvent::Focus: state.focus = true; break;
		case FocusEvent::Blur: state.focus = false; break;
		}
	}
};


void App() {
	// while a normal ViewFrame doesn't support resetting its child view, MutableFrame extends ViewFrame and allows its child view to be reset
	// MutableFrame re-computes the layout and redraws when its child view is reset
	ref_ptr<MutableFrame<Fixed, Fixed>> mutable_frame;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Hot Reload",
			// save the mutable_frame pointer here just as a reference
			// it remains valid until the window is closed
			mutable_frame = new MutableFrame(
				new Placeholder<Fixed, Fixed>()
			)
		)
	);

	EditBoxRef::State edit_box_state;

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
		// the caret blinking state, mouse state and cursor might not update consistently if this is not enabled
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
		struct EditBoxStyle : EditBoxRef::Style {
			EditBoxStyle() {
				font.size(36.0f).color(Color::ForestGreen);
			}
		};

		// hot reload for adding/removing frame templates or changing template arguments is not supported by Visual Studio
		// this is because instantiation of templates introduces new data types
		// we could explicitly instantiate the templates that might be used so that their symbols stay static (see the end of this file)
		ref_ptr<EditBoxRef> edit_box;
		mutable_frame->Reset(
			new BackgroundFrame(
				Color::PaleTurquoise,
				new ClipFrame<Fixed, Fixed, TopLeft>(
					new PaddingFrame(
						Padding(15.0f),
						edit_box = new EditBoxRef(EditBoxStyle(), edit_box_state)
					)
				)
			)
		);
		// EditBoxRef is detached from the view tree when created and SetFocus() / SetCapture() won't work until it is added in the view tree
		edit_box->LoadState();
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
