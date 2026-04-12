#pragma once

#include "../view_traits.h"
#include "../../figure/text_block.h"


namespace ViewDesign {


class TextBox : public ViewType<Relative, Auto> {
public:
	using Style = TextBlockStyle;

public:
	TextBox(Style style, std::wstring text) : style(style), text(text), text_block(style, text) {}
	~TextBox() {}

	// text
protected:
	Style style;
	std::wstring text;
	TextBlock text_block;
public:
	const std::wstring& GetText() const { return text; }
protected:
	virtual void OnTextUpdate() {
		text_block.SetText(style, text);
		if (Size new_size = UpdateLayout(); size == new_size) {
			Redraw(region_infinite);
		} else {
			SizeUpdated(size = new_size);
		}
	}
public:
	void Assign(std::wstring str) { text.assign(std::move(str)); OnTextUpdate(); }
	void Insert(size_t pos, wchar ch) { text.insert(pos, 1, ch); OnTextUpdate(); }
	void Insert(size_t pos, const std::wstring& str) { text.insert(pos, str); OnTextUpdate(); }
	void Replace(TextRange range, wchar ch) { text.replace(range.begin(), range.length(), 1, ch); OnTextUpdate(); }
	void Replace(TextRange range, const std::wstring& str) { text.replace(range.begin(), range.length(), str); OnTextUpdate(); }
	void Erase(size_t pos) { text.erase(pos); OnTextUpdate(); }
	void Erase(TextRange range) { text.erase(range.begin(), range.length()); OnTextUpdate(); }

	// layout
protected:
	float width_ref = 0.0f;
	Size size;
protected:
	Size UpdateLayout() {
		text_block.UpdateSizeRef(Size(width_ref, length_max));
		return text_block.GetSize();
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		width_ref = size_ref.width;
		return size = UpdateLayout();
	}

	// paint
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.draw(point_zero, new TextBlockFigure(text_block, style.font._color));
	}
};


} // namespace ViewDesign
