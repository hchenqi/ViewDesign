#include "ViewDesign/view/control/TextEditor.h"
#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/system/clipboard.h"

#include <utility>
#include <cassert>


namespace ViewDesign {

namespace Stateful {

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


TextEditor::TextEditor(Style style, State& state) :
	Holder<TextView::Style>(std::move(style)), Base(Holder<TextView::Style>::value, state.text),
	style(std::move(style)), state(state), mouse_tracker(state.input.mouse), key_tracker(state.input.key) {
	style.edit._disabled ? ime.Disable(*this) : ime.Enable(*this);
	word_iterator.SetText(text);
	if (state.caret.active && IsCaretBlinking()) { SetCaretTimer(); }
}

TextRange TextEditor::GetCharacterRange(size_t position) const {
	switch (GetU16UnitType(text[position])) {
	case U16UnitType::Single: return TextRange(position, 1);
	case U16UnitType::SurrogateHigh: if (position + 1 < text.length()) { return TextRange(position, 2); } break;
	case U16UnitType::SurrogateLow: if (position > 0) { return TextRange(position - 1, 2); } break;
	}
	throw std::invalid_argument("invalid UTF-16 code point");
}

TextRange TextEditor::GetWordRange(size_t position) const {
	if (text.length() == 0) { return text_range_empty; }
	if (position >= text.length()) { position--; }
	return word_iterator.Seek(position);
}

TextRange TextEditor::GetParagraphRange(size_t position) const {
	size_t length = text.length();
	if (length == 0) { return text_range_empty; }
	if (position >= length) { position = length - 1; }
	size_t begin = position - 1, end = position;
	while (begin < length && text[begin] != u'\n') { begin--; }
	while (end < length && text[end] != u'\n') { end++; }
	begin++; if (end < length) { end++; }
	return TextRange(begin, end - begin);
}

void TextEditor::OnTextUpdate() {
	word_iterator.SetText(text);
	Base::OnTextUpdate();
}

Size TextEditor::OnSizeRefUpdate(Size size_ref) {
	Size size = Base::OnSizeRefUpdate(size_ref);
	UpdateCaret(state.caret.position);
	UpdateSelection(state.selection.current_range);
	UpdateImeComposition(state.input.ime_composition_range);
	return size;
}

void TextEditor::OnDraw(Canvas& canvas, Rect draw_region) {
	Base::OnDraw(canvas, draw_region);
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

void TextEditor::CaretStartBlinking() {
	if (state.caret.active) {
		if (!caret_timer.IsSet()) {
			SetCaretTimer();
		}
		state.caret.track_time = std::chrono::steady_clock::now();
	}
}

void TextEditor::CaretBlink() {
	if (!state.caret.active || std::chrono::steady_clock::now() >= state.caret.track_time + caret_blink_expire_time) {
		caret_timer.Stop();
	}
	Redraw(caret_region);
}

Rect TextEditor::GetCaretRegion(const HitTestPointInfo& info) const {
	const auto& [range, bidi_level, region] = info;
	if ((range.length() == 0) ^ (bidi_level % 2 != 0)) {
		return Rect(region.point, Size(style.edit._caret_width, region.size.height));
	} else {
		return Rect(Point(region.point.x + region.size.width, region.point.y) - Vector(style.edit._caret_width, 0), Size(style.edit._caret_width, region.size.height));
	}
}

void TextEditor::UpdateCaret(const HitTestPointInfo& info) {
	state.caret.position = std::get<TextRange>(info);
	Redraw(std::exchange(caret_region, GetCaretRegion(info)).Union(caret_region));
}

void TextEditor::SetCaret(const HitTestPointInfo& info) {
	UpdateCaret(info);
	ShowCaret();
	ClearSelection();
	state.selection.initial_range = TextRange(state.caret.position.end(), 0);
	state.selection.mode = SelectionMode::Character;
}

void TextEditor::MoveCaret(CaretMoveDirection direction) {
	switch (direction) {
	case CaretMoveDirection::Left:
		if (HasSelection()) {
			SetCaret(TextRange(state.selection.current_range.begin(), 0));
		} else {
			if (state.caret.position.end() > 0) {
				SetCaret(TextRange(state.caret.position.end() - GetPrevCharacterLength(text[state.caret.position.end() - 1]), 0));
			}
		}
		break;
	case CaretMoveDirection::Right:
		if (HasSelection()) {
			SetCaret(TextRange(state.selection.current_range.end(), 0));
		} else {
			if (state.caret.position.end() < text.length()) {
				SetCaret(TextRange(state.caret.position.end() + GetNextCharacterLength(text[state.caret.position.end()]), 0));
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

void TextEditor::UpdateSelection(TextRange range) {
	state.selection.current_range = range;
	selection_region_list.clear();
	Rect selection_region_union = rect_empty;
	if (!state.selection.current_range.empty()) {
		HitTestRangeInfo selection_info = text_block.HitTestRange(state.selection.current_range); selection_region_list.reserve(selection_info.size());
		for (auto& it : selection_info) {
			it.second.size = it.second.size.Union(Size(1.0f, 1.0f));
			selection_region_list.emplace_back(it.second);
			selection_region_union = Rect::Union(selection_region_union, it.second);
		}
	}
	Redraw(std::exchange(this->selection_region_union, selection_region_union).Union(this->selection_region_union));
}

void TextEditor::SelectWord() {
	UpdateSelection(GetWordRange(state.selection.initial_range.begin()));
	state.selection.mode = SelectionMode::Word;
	state.selection.initial_range = state.selection.current_range;
	UpdateCaret(TextRange(state.selection.current_range.end() - 1, 1));
}

void TextEditor::SelectParagraph() {
	UpdateSelection(GetParagraphRange(state.selection.initial_range.begin()));
	state.selection.mode = SelectionMode::Paragraph;
	state.selection.initial_range = state.selection.current_range;
	UpdateCaret(TextRange(state.selection.current_range.end() - 1, 1));
}

void TextEditor::DoSelect(const HitTestPointInfo& info) {
	TextRange current_range;
	switch (state.selection.mode) {
	case SelectionMode::Character: current_range = TextRange(std::get<TextRange>(info).end(), 0); break;
	case SelectionMode::Word: current_range = GetWordRange(std::get<TextRange>(info).begin()); break;
	case SelectionMode::Paragraph: current_range = GetParagraphRange(std::get<TextRange>(info).begin()); break;
	default: assert(false);
	}
	size_t begin = std::min(current_range.begin(), state.selection.initial_range.begin()), end = std::max(current_range.end(), state.selection.initial_range.end());
	UpdateCaret(TextRange(begin, begin < state.selection.initial_range.begin() ? 0 : end - begin));
	UpdateSelection(TextRange(begin, end - begin));
}

void TextEditor::Insert(u16char ch) {
	Operation([&] {
		if (HasSelection()) {
			Base::Replace(state.selection.current_range, ch);
			SetCaret(TextRange(state.selection.current_range.begin() + 1, 0));
		} else {
			Base::Insert(state.caret.position.end(), ch);
			SetCaret(TextRange(state.caret.position.end() + 1, 0));
		}
	});
}

void TextEditor::Insert(u16pair ch) {
	Operation([&] {
		if (HasSelection()) {
			Base::Replace(state.selection.current_range, ch);
			SetCaret(TextRange(state.selection.current_range.begin() + ch.length(), 0));
		} else {
			Base::Insert(state.caret.position.end(), ch);
			SetCaret(TextRange(state.caret.position.end() + ch.length(), 0));
		}
	});
}

void TextEditor::Insert(const u16string& str) {
	Operation([&] {
		if (HasSelection()) {
			Base::Replace(state.selection.current_range, str);
			SetCaret(TextRange(state.selection.current_range.begin() + str.length(), 0));
		} else {
			Base::Insert(state.caret.position.end(), str);
			SetCaret(TextRange(state.caret.position.end() + str.length(), 0));
		}
	});
}

void TextEditor::Delete(bool is_backspace) {
	Operation([&] {
		if (HasSelection()) {
			Base::Erase(state.selection.current_range);
			SetCaret(TextRange(state.selection.current_range.begin(), 0));
		} else {
			if (is_backspace) {
				if (state.caret.position.end() > 0) {
					size_t length = GetPrevCharacterLength(text[state.caret.position.end() - 1]);
					Base::Erase(TextRange(state.caret.position.end() - length, length));
					SetCaret(TextRange(state.caret.position.end() - length, 0));
				}
			} else {
				if (state.caret.position.end() < text.length()) {
					Base::Erase(TextRange(state.caret.position.end(), GetNextCharacterLength(text[state.caret.position.end()])));
				}
			}
		}
	});
}

void TextEditor::UpdateImeComposition(TextRange range) {
	state.input.ime_composition_range = range;
	ime_composition_region_list.clear();
	if (!state.input.ime_composition_range.empty()) {
		HitTestRangeInfo ime_composition_info = text_block.HitTestRange(state.input.ime_composition_range); ime_composition_region_list.reserve(ime_composition_info.size());
		for (auto& it : ime_composition_info) {
			ime_composition_region_list.emplace_back(it.second);
		}
	}
}

void TextEditor::OnImeBegin() {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		UpdateImeComposition(state.selection.current_range);
		ime.SetPosition(*this, selection_region_list.front().BottomRight());
	} else {
		UpdateImeComposition(TextRange(state.caret.position.end(), 0));
		ime.SetPosition(*this, caret_region.BottomRight());
	}
}

void TextEditor::OnImeString() {
	Operation([&] {
		u16string str = ime.GetString();
		Base::Replace(state.input.ime_composition_range, str);
		UpdateImeComposition(TextRange(state.input.ime_composition_range.begin(), str.length()));
		SetCaret(TextRange(state.input.ime_composition_range.begin(), ime.GetCursorPosition()));
	});
}

void TextEditor::OnImeEnd() {
	if (state.caret.position.end() != state.input.ime_composition_range.end()) {
		SetCaret(TextRange(state.input.ime_composition_range.end() - 1, 1));
	}
	ClearImeComposition();
}

void TextEditor::Cut() {
	if (HasSelection()) {
		Copy();
		Delete(false);
	}
}

void TextEditor::Copy() {
	if (HasSelection()) {
		SetClipboardText(text.substr(state.selection.current_range.begin(), state.selection.current_range.length()));
	}
}

void TextEditor::Paste() {
	if (IsEditDisabled()) { return; }
	u16string str; GetClipboardText(str);
	if (!str.empty()) { Insert(str); }
}

void TextEditor::OnMouseEvent(MouseEvent event) {
	switch (event.type) {
	case MouseEvent::LeftDown: SetFocus(); SetCapture(); SetCaret(event.point); break;
	case MouseEvent::LeftUp: ReleaseCapture(); break;
	}
	switch (mouse_tracker.Track(event)) {
	case MouseTrackEvent::LeftDoubleClick: SelectWord(); break;
	case MouseTrackEvent::LeftTripleClick: SelectParagraph(); break;
	case MouseTrackEvent::LeftDrag: DoSelect(event.point); break;
	}
	CaretStartBlinking();
}

void TextEditor::OnKeyEvent(KeyEvent event) {
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

		case Key::Char('A'): if (ctrl()) { SelectAll(); } break;
		case Key::Char('X'): if (ctrl()) { Cut(); } break;
		case Key::Char('C'): if (ctrl()) { Copy(); } break;
		case Key::Char('V'): if (ctrl()) { Paste(); } break;
		}
		break;
	case KeyEvent::Char:
		if (ctrl()) { break; }
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

void TextEditor::OnFocusEvent(FocusEvent event) {
	switch (event) {
	case FocusEvent::Blur: HideCaret(); ClearSelection(); ClearImeComposition(); break;
	}
}


} // namespace Stateful

} // namespace ViewDesign
