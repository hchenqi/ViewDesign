#pragma once

#include "ViewDesign/view/control/TextView.h"
#include "ViewDesign/view/wrapper/Cursor.h"
#include "ViewDesign/common/word_iterator.h"
#include "ViewDesign/event/timer.h"
#include "ViewDesign/event/ime.h"
#include "ViewDesign/event/mouse_tracker.h"
#include "ViewDesign/event/key_tracker.h"


namespace ViewDesign {

namespace Stateful {


class TextEditor : protected Holder<Stateful::TextView::Style>, public CustomizedCursor<CursorStyle::Text, TextViewMutable> {
	// style
protected:
	struct TextEditorStyle {
		struct EditStyle {
		public:
			bool _disabled = false; // can only select and copy if disabled
			float _caret_width = 1.0f;
			Color _caret_color = ColorCode::DimGray;
			Color _selection_color = Color(ColorCode::DimGray, 0x7f);
			float _ime_composition_underline_width = 1.0f;
			Color _ime_composition_underline_color = ColorCode::DimGray;
		public:
			constexpr EditStyle& enable() { _disabled = false; return *this; }
			constexpr EditStyle& disable() { _disabled = true; return *this; }
			constexpr EditStyle& caret_width(float width) { _caret_width = width; return *this; }
			constexpr EditStyle& caret_color(Color color) { _caret_color = color; return *this; }
			constexpr EditStyle& selection_color(Color color) { _selection_color = color; return *this; }
			constexpr EditStyle& ime_composition_underline_width(float width) { _ime_composition_underline_width = width; return *this; }
			constexpr EditStyle& ime_composition_underline_color(Color color) { _ime_composition_underline_color = color; return *this; }
		} edit;
	};
public:
	struct Style : TextView::Style, TextEditorStyle {};

	// state
public:
	struct CaretState {
		bool active = false;
		TextRange position;
		std::chrono::steady_clock::time_point activation_time;
		std::chrono::steady_clock::time_point track_time;
	};

	enum class SelectionMode {
		Character,
		Word,
		Paragraph
	};

	struct SelectionState {
		TextRange current_range = text_range_empty;
		SelectionMode mode;
		TextRange initial_range;
	};

	struct InputState {
		MouseTracker::State mouse;
		KeyTracker::State key;
		TextRange ime_composition_range = text_range_empty;
	};

	struct State {
		u16string text;
		CaretState caret;
		SelectionState selection;
		InputState input;
	};

public:
	TextEditor(Style style, State& state);

	// style
protected:
	TextEditorStyle style;
protected:
	bool IsEditDisabled() const { return style.edit._disabled; }

	// state
protected:
	State& state;
public:
	void OnStateUpdate() {
		OnTextUpdate();
		UpdateCaret(state.caret.position);
		UpdateSelection(state.selection.current_range);
		UpdateImeComposition(state.input.ime_composition_range);
	}

	// text
protected:
	using HitTestPointInfo = TextBlock::HitTestPointInfo;
	using HitTestRangeInfo = TextBlock::HitTestRangeInfo;
protected:
	mutable WordIterator word_iterator;
protected:
	TextRange GetCharacterRange(size_t position) const;
	TextRange GetWordRange(size_t position) const;
	TextRange GetParagraphRange(size_t position) const;
	TextRange GetEntireRange() const { return TextRange(0, text.length()); }
protected:
	virtual void OnTextUpdate() override;

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override;

	// caret
protected:
	static constexpr std::chrono::milliseconds caret_blink_period{ 500 };  // 500ms
	static constexpr std::chrono::milliseconds caret_blink_expire_time{ 20000 };  // 20s
protected:
	Timer caret_timer{ std::bind(&TextEditor::CaretBlink, this) };
	Rect caret_region;
protected:
	bool IsCaretBlinking() const { return std::chrono::steady_clock::now() < state.caret.track_time + caret_blink_expire_time; }
	bool IsCaretVisible() const { return state.caret.active ? IsCaretBlinking() ? ((std::chrono::steady_clock::now() - state.caret.activation_time + caret_blink_period / 2) / caret_blink_period) % 2 == 0 : true : false; }
protected:
	void ShowCaret() { state.caret.active = true; state.caret.activation_time = std::chrono::steady_clock::now(); caret_timer.Stop(); }
	void HideCaret() { if (state.caret.active == true) { state.caret.active = false; Redraw(caret_region); } }
	void SetCaretTimer() { caret_timer.Set(static_cast<unsigned int>(caret_blink_period.count())); }
	void CaretStartBlinking();
	void CaretBlink();
protected:
	Rect GetCaretRegion(const HitTestPointInfo& info) const;
	void UpdateCaret(const HitTestPointInfo& info);
	void UpdateCaret(TextRange position) { UpdateCaret(text_block.HitTestPosition(position)); }
protected:
	void SetCaret(const HitTestPointInfo& info);
	void SetCaret(Point point) { SetCaret(text_block.HitTestPoint(point)); }
	void SetCaret(TextRange position) { SetCaret(text_block.HitTestPosition(position)); }
protected:
	enum class CaretMoveDirection { Left, Right, Up, Down, Home, End };
protected:
	void MoveCaret(CaretMoveDirection direction);

	// selection
protected:
	std::vector<Rect> selection_region_list;
	Rect selection_region_union;
protected:
	bool HasSelection() const { return !state.selection.current_range.empty(); }
	void UpdateSelection(TextRange range);
	void ClearSelection() { UpdateSelection(text_range_empty); }
protected:
	void SelectWord();
	void SelectParagraph();
	void SelectAll() { UpdateSelection(GetEntireRange()); }
protected:
	void DoSelect(const HitTestPointInfo& info);
	void DoSelect(Point current_point) { DoSelect(text_block.HitTestPoint(current_point)); }
	void DoSelect(TextRange current_position) { DoSelect(text_block.HitTestPosition(current_position)); }

	// input
protected:
	void Insert(u16char ch);
	void Insert(u16pair ch);
	void Insert(const u16string& str);
	void Delete(bool is_backspace);

	// ime
protected:
	std::vector<Rect> ime_composition_region_list;
protected:
	bool HasImeComposition() { return !state.input.ime_composition_range.empty(); }
	void UpdateImeComposition(TextRange range);
	void ClearImeComposition() { UpdateImeComposition(text_range_empty); }
protected:
	void OnImeBegin();
	void OnImeString();
	void OnImeEnd();

	// clipboard
protected:
	void Cut();
	void Copy();
	void Paste();

	// control
public:
	void Edit(size_t position = -1) {
		SetCaret(TextRange(position, 0));
		SetFocus();
		CaretStartBlinking();
	}

	// event
protected:
	MouseTracker mouse_tracker;
	KeyTracker key_tracker;
protected:
	bool ctrl() const { return state.input.key.ctrl; }
	bool shift() const { return state.input.key.shift; }
protected:
	virtual void OnMouseEvent(MouseEvent event) override;
	virtual void OnKeyEvent(KeyEvent event) override;
	virtual void OnFocusEvent(FocusEvent event) override;
};


} // namespace Stateful


class TextEditor : protected Holder<Stateful::TextEditor::State>, public Stateful::TextEditor {
public:
	TextEditor(Style style, u16string text = u"") : Holder<State>(std::move(text)), Stateful::TextEditor(std::move(style), Holder<State>::value) {}
};


} // namespace ViewDesign
