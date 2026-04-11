#include "EditBox.h"
#include "../../figure/shape.h"
#include "../../system/clipboard.h"


namespace ViewDesign {


EditBox::EditBox(Style style, std::wstring text) : Base(style, text), style(style) {
	style.edit._disabled ? ime.Disable(*this) : ime.Enable(*this);
	word_break_iterator.SetText(this->text);
}

size_t EditBox::GetCharacterLength(size_t position) const { return GetUTF16CharLength(text[position]); }

TextRange EditBox::GetWordRange(size_t position) const {
	if (text.length() == 0) { return text_range_empty; }
	if (position >= text.length()) { position--; }
	return word_break_iterator.Seek(position);
}

TextRange EditBox::GetParagraphRange(size_t position) const {
	if (text.length() == 0) { return text_range_empty; }
	size_t length = text.length(); if (position >= length) { position--; }
	size_t begin = position - 1, end = position;
	while (begin < length && text[begin] != L'\n') { begin--; }
	while (end < length && text[end] != L'\n') { end++; }
	begin++; end++;
	return TextRange(begin, end - begin);
}

void EditBox::OnTextUpdate() { word_break_iterator.SetText(text); TextBox::OnTextUpdate(); }

Size EditBox::OnSizeRefUpdate(Size size_ref) {
	Size size = TextBox::OnSizeRefUpdate(size_ref);
	UpdateCaret(caret_position);
	UpdateSelection(selection_range);
	return size;
}

void EditBox::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	TextBox::OnDraw(figure_queue, draw_region);
	if (IsCaretVisible()) {
		figure_queue.add(caret_region.point, new Rectangle(caret_region.size, style.edit._caret_color));
	}
	if (HasSelection()) {
		for (auto& rect : selection_region_list) {
			figure_queue.add(rect.point, new Rectangle(rect.size, style.edit._selection_color));
		}
	}
	if (HasImeComposition()) {
		for (auto& rect : ime_composition_region_list) {
			figure_queue.add(Point(rect.left(), rect.top() + rect.height() - ime_composition_underline_height), new Rectangle(Size(rect.width(), ime_composition_underline_height), style.edit._ime_composition_underline_color));
		}
	}
}

void EditBox::UpdateCaret(const HitTestInfo& info) {
	caret_position = info.range.begin();
	Rect caret_region_old = caret_region; caret_region = Rect(info.region.point, Size(caret_width, info.region.size.height));
	Redraw(caret_region_old.Union(caret_region));
}

void EditBox::SetCaret(const HitTestInfo& info) {
	UpdateCaret(info);
	caret_state = CaretState::Show;
	ClearSelection();
	selection_initial_range = TextRange(caret_position, 0);
	selection_mode = SelectionMode::Character;
}

void EditBox::MoveCaret(CaretMoveDirection direction) {
	switch (direction) {
	case CaretMoveDirection::Left:
		if (HasSelection()) {
			SetCaret(selection_range.begin());
		} else {
			if (caret_position > 0) {
				SetCaret(caret_position - 1);
			}
		}
		break;
	case CaretMoveDirection::Right:
		if (HasSelection()) {
			SetCaret(selection_range.end());
		} else {
			SetCaret(caret_position + GetCharacterLength(caret_position));
		}
		break;
	case CaretMoveDirection::Up:
		SetCaret(caret_region.Center() - Vector(0, caret_region.size.height));
		break;
	case CaretMoveDirection::Down:
		SetCaret(caret_region.Center() + Vector(0, caret_region.size.height));
		break;
	case CaretMoveDirection::Home:
		SetCaret(Point(0, caret_region.Center().y));
		break;
	case CaretMoveDirection::End:
		SetCaret(Point(position_max, caret_region.Center().y));
		break;
	}
}

void EditBox::HideCaret() {
	if (caret_state != CaretState::Hide) {
		caret_state = CaretState::Hide;
		Redraw(caret_region);
	}
}

void EditBox::StartBlinkingCaret() {
	if (caret_state != CaretState::Hide) {
		if (!caret_timer.IsSet()) {
			caret_timer.Set(caret_blink_period);
		}
		caret_blink_time = 0;
	}
}

void EditBox::BlinkCaret() {
	caret_blink_time += caret_blink_period;
	if (caret_blink_time >= caret_blink_expire_time) {
		caret_state = CaretState::Show;
		caret_timer.Stop();
		return;
	}
	switch (caret_state) {
	case CaretState::Hide: caret_timer.Stop(); return;
	case CaretState::Show:
	case CaretState::BlinkShow: caret_state = CaretState::BlinkHide; break;
	case CaretState::BlinkHide: caret_state = CaretState::BlinkShow; break;
	}
	Redraw(caret_region);
}

void EditBox::UpdateSelection(TextRange range) {
	selection_range = range;
	selection_region_list.clear();
	Rect selection_region_union_old = selection_region_union; selection_region_union = region_empty;
	if (!selection_range.empty()) {
		std::vector<HitTestInfo> selection_info = text_block.HitTestRange(selection_range); selection_region_list.reserve(selection_info.size());
		for (auto& it : selection_info) {
			selection_region_list.emplace_back(it.region);
			selection_region_union = Rect::Union(selection_region_union, it.region);
		}
	}
	Redraw(selection_region_union_old.Union(selection_region_union));
}

void EditBox::SelectWord() {
	UpdateSelection(GetWordRange(selection_initial_range.begin()));
	selection_mode = SelectionMode::Word;
	selection_initial_range = selection_range;
	UpdateCaret(selection_range.end());
}

void EditBox::SelectParagraph() {
	UpdateSelection(GetParagraphRange(selection_initial_range.begin()));
	selection_mode = SelectionMode::Paragraph;
	selection_initial_range = selection_range;
	UpdateCaret(selection_range.end());
}

void EditBox::DoSelect(const HitTestInfo& info) {
	TextRange current_range;
	switch (selection_mode) {
	case SelectionMode::Character: current_range = TextRange(info.range.begin(), 0); break;
	case SelectionMode::Word: current_range = GetWordRange(info.range.begin()); break;
	case SelectionMode::Paragraph: current_range = GetParagraphRange(info.range.begin()); break;
	}
	size_t begin = min(current_range.begin(), selection_initial_range.begin()), end = max(current_range.end(), selection_initial_range.end());
	UpdateCaret(begin < selection_initial_range.begin() ? begin : end);
	UpdateSelection(TextRange(begin, end - begin));
}

void EditBox::Insert(wchar ch) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Replace(selection_range, ch);
		SetCaret(selection_range.begin() + 1);
	} else {
		TextBox::Insert(caret_position, ch);
		SetCaret(caret_position + 1);
	}
}

void EditBox::Insert(const std::wstring& str) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Replace(selection_range, str);
		SetCaret(selection_range.begin() + str.length());
	} else {
		TextBox::Insert(caret_position, str);
		SetCaret(caret_position + str.length());
	}
}

void EditBox::Delete(bool is_backspace) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Erase(selection_range);
		SetCaret(selection_range.begin());
	} else {
		if (is_backspace) {
			if (caret_position == 0) { return; }
			size_t caret_position_old = caret_position;
			SetCaret(caret_position - 1);
			TextBox::Erase(TextRange(caret_position, caret_position_old - caret_position));
		} else {
			if (caret_position >= text.length()) { return; }
			TextBox::Erase(TextRange(caret_position, GetCharacterLength(caret_position)));
		}
	}
}

void EditBox::UpdateImeComposition(TextRange range) {
	ime_composition_range = range;
	ime_composition_region_list.clear();
	if (!ime_composition_range.empty()) {
		std::vector<HitTestInfo> ime_composition_info = text_block.HitTestRange(ime_composition_range); ime_composition_region_list.reserve(ime_composition_info.size());
		for (auto& it : ime_composition_info) {
			ime_composition_region_list.emplace_back(it.region);
		}
	}
}

void EditBox::OnImeBegin() {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		UpdateImeComposition(selection_range);
		ime.SetPosition(*this, selection_region_list.front().RightBottom());
	} else {
		UpdateImeComposition(TextRange(caret_position, 0));
		ime.SetPosition(*this, caret_region.RightBottom());
	}
}

void EditBox::OnImeString() {
	if (IsEditDisabled()) { return; }
	std::wstring str = ime.GetString();
	TextBox::Replace(ime_composition_range, str);
	UpdateImeComposition(TextRange(ime_composition_range.begin(), str.length()));
	SetCaret(ime_composition_range.begin() + ime.GetCursorPosition());
}

void EditBox::OnImeEnd() {
	if (caret_position != ime_composition_range.end()) { SetCaret(ime_composition_range.end()); }
	ClearImeComposition();
}

void EditBox::Cut() {
	if (HasSelection()) {
		Copy();
		Delete(false);
	}
}

void EditBox::Copy() {
	if (HasSelection()) {
		SetClipboardData(Substr(selection_range));
	}
}

void EditBox::Paste() {
	if (IsEditDisabled()) { return; }
	std::wstring str; GetClipboardData(str);
	if (!str.empty()) { Insert(str); }
}

void EditBox::OnMouseEvent(MouseEvent event) {
	switch (event.type) {
	case MouseEvent::LeftDown: SetCaret(event.point); SetFocus(); SetCapture(); break;
	case MouseEvent::LeftUp: ReleaseCapture(); break;
	}
	switch (mouse_tracker.Track(event)) {
	case MouseTrackEvent::LeftDoubleClick: SelectWord(); break;
	case MouseTrackEvent::LeftTripleClick: SelectParagraph(); break;
	case MouseTrackEvent::LeftDrag: DoSelect(event.point); break;
	}
	StartBlinkingCaret();
}

void EditBox::OnKeyEvent(KeyEvent event) {
	key_tracker.Track(event);
	switch (event.type) {
	case KeyEvent::KeyDown:
		switch (event.key) {
		case Key::Left: MoveCaret(CaretMoveDirection::Left); break;
		case Key::Right: MoveCaret(CaretMoveDirection::Right); break;
		case Key::Up: MoveCaret(CaretMoveDirection::Up); break;
		case Key::Down: MoveCaret(CaretMoveDirection::Down); break;
		case Key::Home: MoveCaret(CaretMoveDirection::Home); break;
		case Key::End: MoveCaret(CaretMoveDirection::End); break;

		case Key::Enter: Insert(L'\n'); break;
		case Key::Tab: Insert(L'\t'); break;

		case Key::Backspace: Delete(true); break;
		case Key::Delete: Delete(false); break;

		case CharKey('A'): if (key_tracker.ctrl) { SelectAll(); } break;
		case CharKey('X'): if (key_tracker.ctrl) { Cut(); } break;
		case CharKey('C'): if (key_tracker.ctrl) { Copy(); } break;
		case CharKey('V'): if (key_tracker.ctrl) { Paste(); } break;
		}
		break;
	case KeyEvent::Char:
		if (key_tracker.ctrl) { break; }
		if (!iswcntrl(event.ch)) { Insert(event.ch); };
		break;
	case KeyEvent::ImeBegin: OnImeBegin(); break;
	case KeyEvent::ImeString: OnImeString(); break;
	case KeyEvent::ImeEnd: OnImeEnd(); break;
	}
	StartBlinkingCaret();
}

void EditBox::OnFocusEvent(FocusEvent event) {
	switch (event) {
	case FocusEvent::Blur: HideCaret(); ClearSelection(); ClearImeComposition(); break;
	}
}


} // namespace ViewDesign
