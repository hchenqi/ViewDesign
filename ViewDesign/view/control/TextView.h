#pragma once

#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/view/figure/text_block.h"
#include "ViewDesign/common/holder.h"


namespace ViewDesign {

namespace Stateful {


class TextView : public ViewBase, public SizeTrait<Relative, Relative> {
public:
	using Style = TextStyle;

public:
	TextView(const Style& style, const u16string& text) : style(style), text(text) { text_block.SetText(style, text); }

	// state
protected:
	const Style& style;
	const u16string& text;
public:
	const Style& GetStyle() const { return style; }
	const u16string& GetText() const { return text; }
protected:
	virtual void OnTextUpdate() {
		text_block.SetText(style, text);
		Rect region = text_block.UpdateLayout(size_ref);
		if (this->region.size != region.size) {
			this->region = region;
			SizeUpdated(this->region.size);
		} else {
			this->region = region;
			Redraw(rect_infinite);
		}
	}

	// layout
protected:
	TextBlock text_block;
	Size size_ref = size_empty;
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
		canvas.draw(point_zero + (point_zero - region.point), new TextBlockFigure(text_block, style.font._color));
	}
};


class TextViewMutable : public TextView {
public:
	TextViewMutable(Style& style, u16string& text) : TextView(style, text) {}

	// state
public:
	TextView::GetStyle;
	TextView::GetText;
public:
	Style& GetStyle() { return const_cast<Style&>(TextView::GetStyle()); }
	u16string& GetText() { return const_cast<u16string&>(TextView::GetText());; }
public:
	void Assign(u16string str) { GetText().assign(std::move(str)); OnTextUpdate(); }
	void Insert(size_t pos, u16char ch) { GetText().insert(pos, 1, ch); OnTextUpdate(); }
	void Insert(size_t pos, u16pair ch) {
		if (size_t length = ch.length(); length > 0) {
			if (length == 1) {
				GetText().insert(pos, 1, ch.first);
			} else {
				GetText().insert(GetText().begin() + pos, { ch.first, ch.second });
			}
			OnTextUpdate();
		}
	}
	void Insert(size_t pos, const u16string& str) { GetText().insert(pos, str); OnTextUpdate(); }
	void Replace(TextRange range, u16char ch) { GetText().replace(range.begin(), range.length(), 1, ch); OnTextUpdate(); }
	void Replace(TextRange range, u16pair ch) {
		if (size_t length = ch.length(); length > 0) {
			if (length == 1) {
				GetText().replace(range.begin(), range.length(), 1, ch.first);
			} else {
				GetText().replace(GetText().begin() + range.begin(), GetText().begin() + range.end(), { ch.first, ch.second });
			}
		} else {
			GetText().erase(range.begin(), range.length());
		}
		OnTextUpdate();
	}
	void Replace(TextRange range, const u16string& str) { GetText().replace(range.begin(), range.length(), str); OnTextUpdate(); }
	void Erase(size_t pos) { GetText().erase(pos); OnTextUpdate(); }
	void Erase(TextRange range) { GetText().erase(range.begin(), range.length()); OnTextUpdate(); }
};


} // namespace Stateful


class TextView : protected Holder<Stateful::TextView::Style>, protected Holder<u16string>, public Stateful::TextViewMutable {
public:
	TextView(Style style, u16string text) : Holder<Style>(std::move(style)), Holder<u16string>(std::move(text)), TextViewMutable(Holder<Style>::value, Holder<u16string>::value) {}
};


} // namespace ViewDesign
