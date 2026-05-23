#pragma once

#include "ViewDesign/view/style/text.h"
#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/common/text_range.h"
#include "ViewDesign/drawing/figure.h"

#include <vector>


namespace ViewDesign {


class TextBlock : Uncopyable {
private:
	friend struct TextBlockFigure;
public:
	TextBlock();
	~TextBlock();
protected:
	Handle layout;
public:
	void SetText(const TextStyle& style, const u16string& text);
	Rect UpdateLayout(Size size_ref);
public:
	using HitTestPointInfo = std::pair<TextRange, Rect>;
	using HitTestRangeInfo = std::vector<std::pair<TextRange, Rect>>;
public:
	HitTestPointInfo HitTestPoint(Point point) const;
	HitTestPointInfo HitTestPosition(TextRange position) const;
	HitTestRangeInfo HitTestRange(TextRange range) const;
};


struct TextBlockFigure : Figure {
	const TextBlock& text_block;
	Color font_color;

	TextBlockFigure(const TextBlock& text_block, Color font_color) : text_block(text_block), font_color(font_color) {}
	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign
