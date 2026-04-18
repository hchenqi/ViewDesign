#pragma once

#include "TextBox.h"
#include "../../common/unicode_helper.h"
#include "../../event/timer.h"
#include "../../event/ime.h"
#include "../../event/mouse_tracker.h"
#include "../../event/key_tracker.h"
#include "../wrapper/Cursor.h"


namespace ViewDesign {


class EditBox : public CustomizedCursor<TextBox, Cursor::Text> {
protected:
	struct EditBoxStyle {
		struct EditStyle {
		public:
			bool _disabled = false;  // can only select and copy if disabled
			Color _caret_color = Color::DimGray;
			Color _selection_color = Color(Color::DimGray, 0x7f);
			Color _ime_composition_underline_color = Color::DimGray;
		public:
			constexpr EditStyle& enable() { _disabled = false; return *this; }
			constexpr EditStyle& disable() { _disabled = true; return *this; }
			constexpr EditStyle& caret_color(Color color) { _caret_color = color; return *this; }
			constexpr EditStyle& selection_color(Color color) { _selection_color = color; return *this; }
			constexpr EditStyle& ime_composition_underline_color(Color color) { _ime_composition_underline_color = color; return *this; }
		}edit;
	};
public:
	struct Style : TextBox::Style, EditBoxStyle {};

public:
	EditBox(Style style, std::wstring text = L"");
	~EditBox() {}

	// style
protected:
	EditBoxStyle style;
protected:
	bool IsEditDisabled() const { return style.edit._disabled; }

	// text
protected:
	using HitTestInfo = TextBlock::HitTestInfo;
protected:
	mutable WordBreakIterator word_break_iterator;
protected:
	size_t GetCharacterLength(size_t position) const;
	TextRange GetWordRange(size_t position) const;
	TextRange GetParagraphRange(size_t position) const;
	TextRange GetEntireRange() const { return TextRange(0, text.length()); }
protected:
	virtual void OnTextUpdate() override;

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;

	// paint
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override;

	// caret
protected:
	static constexpr float caret_width = 1.0f;
	enum class CaretMoveDirection { Left, Right, Up, Down, Home, End };
protected:
	size_t caret_position = 0;
	Rect caret_region = region_empty;
protected:
	void UpdateCaret(const HitTestInfo& info);
	void UpdateCaret(size_t position) { UpdateCaret(text_block.HitTestPosition(position)); }
protected:
	void SetCaret(const HitTestInfo& info);
	void SetCaret(Point point) { SetCaret(text_block.HitTestPoint(point)); }
	void SetCaret(size_t position) { SetCaret(text_block.HitTestPosition(position)); }
	void MoveCaret(CaretMoveDirection direction);

	// caret state
protected:
	static constexpr ushort caret_blink_period = 500;  // 500ms
	static constexpr ushort caret_blink_expire_time = 20000;  // 20s
	enum class CaretState : ushort { Hide, Show, BlinkShow, BlinkHide };
protected:
	Timer caret_timer = Timer(std::bind(&EditBox::CaretBlink, this));
	CaretState caret_state = CaretState::Hide;
	ushort caret_blink_time = 0;
protected:
	bool IsCaretVisible() const { return caret_state == CaretState::Show || caret_state == CaretState::BlinkShow; }
protected:
	void HideCaret();
	void CaretStartBlinking();
	void CaretBlink();

	// selection
protected:
	enum class SelectionMode { Character, Word, Paragraph };
protected:
	SelectionMode selection_mode = SelectionMode::Character;
	TextRange selection_initial_range;
protected:
	TextRange selection_range;
	std::vector<Rect> selection_region_list;
	Rect selection_region_union;
protected:
	bool HasSelection() const { return !selection_range.empty(); }
	void UpdateSelection(TextRange range);
	void ClearSelection() { UpdateSelection(text_range_empty); }
protected:
	void SelectWord();
	void SelectParagraph();
	void SelectAll() { UpdateSelection(GetEntireRange()); }
protected:
	void DoSelect(const HitTestInfo& info);
	void DoSelect(Point current_point) { DoSelect(text_block.HitTestPoint(current_point)); }
	void DoSelect(size_t current_position) { DoSelect(text_block.HitTestPosition(current_position)); }

	// keyboard input
protected:
	void Insert(wchar ch);
	void Insert(const std::wstring& str);
	void Delete(bool is_backspace);

	// ime input
protected:
	static constexpr float ime_composition_underline_height = 1.0f;
protected:
	TextRange ime_composition_range;
	std::vector<Rect> ime_composition_region_list;
protected:
	bool HasImeComposition() { return !ime_composition_range.empty(); }
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

	// event
protected:
	MouseTracker mouse_tracker;
	KeyTracker key_tracker;
protected:
	virtual void OnMouseEvent(MouseEvent event) override;
	virtual void OnKeyEvent(KeyEvent event) override;
	virtual void OnFocusEvent(FocusEvent event) override;
};


} // namespace ViewDesign
