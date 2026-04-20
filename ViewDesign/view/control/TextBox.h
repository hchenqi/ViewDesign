#pragma once

#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/drawing/text_block.h"


namespace ViewDesign {


class TextBox : public ViewType<Relative, Relative> {
public:
	using Style = TextBlockStyle;

public:
	TextBox(Style style, std::wstring text) : style(style), text(text) { text_block.SetText(style, text); }
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
		Rect region = text_block.UpdateLayout(size_ref);
		if (this->region.size != region.size) {
			this->region = region;
			SizeUpdated(this->region.size);
		} else {
			this->region = region;
			Redraw(region_infinite);
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
	Size size_ref;
	Rect region;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (this->size_ref != size_ref) {
			this->size_ref = size_ref;
			region = text_block.UpdateLayout(size_ref);
		}
		return region.size;
	}

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.draw(-region.point, new TextBlockFigure(text_block, style.font._color));
	}
};


} // namespace ViewDesign
