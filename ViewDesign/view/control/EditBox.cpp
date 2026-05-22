#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/drawing/shape.h"
#include "ViewDesign/system/clipboard.h"

#include <utility>


namespace ViewDesign {

namespace {

constexpr size_t GetNextCharacterLength(u16char ch) {
	switch (GetU16UnitType(ch)) {
	case U16UnitType::Single: return 1;
	case U16UnitType::SurrogateHigh: return 2;
	default: throw std::invalid_argument("not begin of a UTF-16 code point");
	}
}

constexpr size_t GetPrevCharacterLength(u16char ch) {
	switch (GetU16UnitType(ch)) {
	case U16UnitType::Single: return 1;
	case U16UnitType::SurrogateLow: return 2;
	default: throw std::invalid_argument("not end of a UTF-16 code point");
	}
}

} // namespace


EditBox::EditBox(const Style& style, u16string text) : Base(style, std::move(text)), style(style) {
	style.edit._disabled ? ime.Disable(*this) : ime.Enable(*this);
	word_iterator.SetText(this->text);
}

TextRange EditBox::GetCharacterRange(size_t position) const {
	switch (GetU16UnitType(text[position])) {
	case U16UnitType::Single: return TextRange(position, 1);
	case U16UnitType::SurrogateHigh: if (position + 1 < text.length()) { return TextRange(position, 2); } break;
	case U16UnitType::SurrogateLow: if (position > 0) { return TextRange(position - 1, 2); } break;
	}
	throw std::invalid_argument("invalid UTF-16 code point");
}

TextRange EditBox::GetWordRange(size_t position) const {
	if (text.length() == 0) { return text_range_empty; }
	if (position >= text.length()) { position--; }
	return word_iterator.Seek(position);
}

TextRange EditBox::GetParagraphRange(size_t position) const {
	size_t length = text.length();
	if (length == 0) { return text_range_empty; }
	if (position >= length) { position = length - 1; }
	size_t begin = position - 1, end = position;
	while (begin < length && text[begin] != u'\n') { begin--; }
	while (end < length && text[end] != u'\n') { end++; }
	begin++; if (end < length) { end++; }
	return TextRange(begin, end - begin);
}

void EditBox::OnTextUpdate() {
	word_iterator.SetText(text);
	TextBox::OnTextUpdate();
}

Size EditBox::OnSizeRefUpdate(Size size_ref) {
	Size size = TextBox::OnSizeRefUpdate(size_ref);
	UpdateCaret(caret_position);
	UpdateSelection(selection_range);
	return size;
}

void EditBox::OnDraw(Canvas& canvas, Rect draw_region) {
	TextBox::OnDraw(canvas, draw_region);
	if (IsCaretVisible()) {
		canvas.draw(caret_region.point, new Rectangle(caret_region.size, style.edit._caret_color));
	}
	if (HasSelection()) {
		for (auto& rect : selection_region_list) {
			canvas.draw(rect.point, new Rectangle(rect.size, style.edit._selection_color));
		}
	}
	if (HasImeComposition()) {
		for (auto& rect : ime_composition_region_list) {
			canvas.draw(Point(rect.left(), rect.top() + rect.height() - style.edit._ime_composition_underline_width), new Rectangle(Size(rect.width(), style.edit._ime_composition_underline_width), style.edit._ime_composition_underline_color));
		}
	}
}

Rect EditBox::GetCaretRegion(const HitTestPointInfo& info) const {
	const auto& [range, region] = info;
	if (range.length() == 0) {
		return Rect(region.point, Size(style.edit._caret_width, region.size.height));
	} else {
		return Rect(Point(region.point.x + region.size.width, region.point.y) - Vector(style.edit._caret_width, 0), Size(style.edit._caret_width, region.size.height));
	}
}

void EditBox::UpdateCaret(const HitTestPointInfo& info) {
	caret_position = info.first;
	Redraw(std::exchange(caret_region, GetCaretRegion(info)).Union(caret_region));
}

void EditBox::SetCaret(const HitTestPointInfo& info) {
	UpdateCaret(info);
	caret_state = CaretState::Show;
	ClearSelection();
	selection_initial_range = TextRange(caret_position.end(), 0);
	selection_mode = SelectionMode::Character;
}

void EditBox::MoveCaret(CaretMoveDirection direction) {
	switch (direction) {
	case CaretMoveDirection::Left:
		if (HasSelection()) {
			SetCaret(TextRange(selection_range.begin(), 0));
		} else {
			if (caret_position.end() > 0) {
				SetCaret(TextRange(caret_position.end() - GetPrevCharacterLength(text[caret_position.end() - 1]), 0));
			}
		}
		break;
	case CaretMoveDirection::Right:
		if (HasSelection()) {
			SetCaret(TextRange(selection_range.end(), 0));
		} else {
			if (caret_position.end() < text.length()) {
				SetCaret(TextRange(caret_position.end() + GetNextCharacterLength(text[caret_position.end()]), 0));
			}
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

void EditBox::CaretStartBlinking() {
	if (caret_state != CaretState::Hide) {
		if (!caret_timer.IsSet()) {
			caret_timer.Set(caret_blink_period);
		}
		caret_blink_time = 0;
	}
}

void EditBox::CaretBlink() {
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
	Rect selection_region_union = rect_empty;
	if (!selection_range.empty()) {
		HitTestRangeInfo selection_info = text_block.HitTestRange(selection_range); selection_region_list.reserve(selection_info.size());
		for (auto& it : selection_info) {
			it.second.size = it.second.size.Union(Size(5.0f, 5.0f));
			selection_region_list.emplace_back(it.second);
			selection_region_union = Rect::Union(selection_region_union, it.second);
		}
	}
	Redraw(std::exchange(this->selection_region_union, selection_region_union).Union(this->selection_region_union));
}

void EditBox::SelectWord() {
	UpdateSelection(GetWordRange(selection_initial_range.begin()));
	selection_mode = SelectionMode::Word;
	selection_initial_range = selection_range;
	UpdateCaret(TextRange(selection_range.end() - 1, 1));
}

void EditBox::SelectParagraph() {
	UpdateSelection(GetParagraphRange(selection_initial_range.begin()));
	selection_mode = SelectionMode::Paragraph;
	selection_initial_range = selection_range;
	UpdateCaret(TextRange(selection_range.end() - 1, 1));
}

void EditBox::DoSelect(const HitTestPointInfo& info) {
	TextRange current_range = text_range_empty;
	switch (selection_mode) {
	case SelectionMode::Character: current_range = TextRange(info.first.end(), 0); break;
	case SelectionMode::Word: current_range = GetWordRange(info.first.begin()); break;
	case SelectionMode::Paragraph: current_range = GetParagraphRange(info.first.begin()); break;
	}
	size_t begin = std::min(current_range.begin(), selection_initial_range.begin()), end = std::max(current_range.end(), selection_initial_range.end());
	UpdateCaret(TextRange(begin, begin < selection_initial_range.begin() ? 0 : end - begin));
	UpdateSelection(TextRange(begin, end - begin));
}

void EditBox::Insert(u16char ch) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Replace(selection_range, ch);
		SetCaret(TextRange(selection_range.begin() + 1, 0));
	} else {
		TextBox::Insert(caret_position.end(), ch);
		SetCaret(TextRange(caret_position.end() + 1, 0));
	}
}

void EditBox::Insert(u16pair ch) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Replace(selection_range, ch);
		SetCaret(TextRange(selection_range.begin() + ch.length(), 0));
	} else {
		TextBox::Insert(caret_position.end(), ch);
		SetCaret(TextRange(caret_position.end() + ch.length(), 0));
	}
}

void EditBox::Insert(const u16string& str) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Replace(selection_range, str);
		SetCaret(TextRange(selection_range.begin() + str.length(), 0));
	} else {
		TextBox::Insert(caret_position.end(), str);
		SetCaret(TextRange(caret_position.end() + str.length(), 0));
	}
}

void EditBox::Delete(bool is_backspace) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		TextBox::Erase(selection_range);
		SetCaret(TextRange(selection_range.begin(), 0));
	} else {
		if (is_backspace) {
			if (caret_position.end() > 0) {
				size_t length = GetPrevCharacterLength(text[caret_position.end() - 1]);
				TextBox::Erase(TextRange(caret_position.end() - length, length));
				SetCaret(TextRange(caret_position.end() - length, 0));
			}
		} else {
			if (caret_position.end() < text.length()) {
				TextBox::Erase(TextRange(caret_position.end(), GetNextCharacterLength(text[caret_position.end()])));
			}
		}
	}
}

void EditBox::UpdateImeComposition(TextRange range) {
	ime_composition_range = range;
	ime_composition_region_list.clear();
	if (!ime_composition_range.empty()) {
		HitTestRangeInfo ime_composition_info = text_block.HitTestRange(ime_composition_range); ime_composition_region_list.reserve(ime_composition_info.size());
		for (auto& it : ime_composition_info) {
			ime_composition_region_list.emplace_back(it.second);
		}
	}
}

void EditBox::OnImeBegin() {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		UpdateImeComposition(selection_range);
		ime.SetPosition(*this, selection_region_list.front().RightBottom());
	} else {
		UpdateImeComposition(TextRange(caret_position.end(), 0));
		ime.SetPosition(*this, caret_region.RightBottom());
	}
}

void EditBox::OnImeString() {
	if (IsEditDisabled()) { return; }
	u16string str = ime.GetString();
	TextBox::Replace(ime_composition_range, str);
	UpdateImeComposition(TextRange(ime_composition_range.begin(), str.length()));
	SetCaret(TextRange(ime_composition_range.begin(), ime.GetCursorPosition()));
}

void EditBox::OnImeEnd() {
	if (caret_position.end() != ime_composition_range.end()) {
		SetCaret(TextRange(ime_composition_range.end() - 1, 1));
	}
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
		SetClipboardText(text.substr(selection_range.begin(), selection_range.length()));
	}
}

void EditBox::Paste() {
	if (IsEditDisabled()) { return; }
	u16string str; GetClipboardText(str);
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
	CaretStartBlinking();
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

		case Key::Enter: Insert(u'\n'); break;
		case Key::Tab: Insert(u'\t'); break;

		case Key::Backspace: Delete(true); break;
		case Key::Delete: Delete(false); break;

		case Key::Char('A'): if (key_tracker.ctrl) { SelectAll(); } break;
		case Key::Char('X'): if (key_tracker.ctrl) { Cut(); } break;
		case Key::Char('C'): if (key_tracker.ctrl) { Copy(); } break;
		case Key::Char('V'): if (key_tracker.ctrl) { Paste(); } break;
		}
		break;
	case KeyEvent::Char:
		if (key_tracker.ctrl) { break; }
		if (event.ch.empty()) { break; }
		if (event.ch.single()) { if (!iswcntrl(event.ch.first)) { Insert(event.ch.first); } break; }
		Insert(event.ch);
		break;
	case KeyEvent::ImeBegin: OnImeBegin(); break;
	case KeyEvent::ImeString: OnImeString(); break;
	case KeyEvent::ImeEnd: OnImeEnd(); break;
	}
	CaretStartBlinking();
}

void EditBox::OnFocusEvent(FocusEvent event) {
	switch (event) {
	case FocusEvent::Blur: HideCaret(); ClearSelection(); ClearImeComposition(); break;
	}
}


} // namespace ViewDesign
