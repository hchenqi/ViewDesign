#pragma once

#include "figure.h"
#include "../common/uncopyable.h"
#include "../style/text_block_style.h"
#include "../geometry/text_range.h"

#include <vector>


namespace ViewDesign {

struct TextLayout;


class TextBlock : Uncopyable {
private:
	friend struct TextBlockFigure;
public:
	TextBlock(const TextBlockStyle& style, const std::wstring& text) : layout(nullptr) { SetText(style, text); }
	~TextBlock();
protected:
	alloc_ptr<TextLayout> layout;
public:
	void SetText(const TextBlockStyle& style, const std::wstring& text);
	void UpdateSizeRef(Size size_ref);
	Size GetSize() const;
public:
	struct HitTestInfo {
		TextRange range;
		Rect region;
	};
public:
	HitTestInfo HitTestPoint(Point point) const;
	HitTestInfo HitTestPosition(size_t position) const;
	std::vector<HitTestInfo> HitTestRange(TextRange range) const;
};


struct TextBlockFigure : Figure {
	const TextBlock& text_block;
	Color font_color;

	TextBlockFigure(const TextBlock& text_block, Color font_color) : text_block(text_block), font_color(font_color) {}
	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign
