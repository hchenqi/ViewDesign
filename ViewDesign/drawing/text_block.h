#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/common/text_range.h"
#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/style/text_block_style.h"

#include <vector>


namespace ViewDesign {

struct TextLayout;


class TextBlock : Uncopyable {
private:
	friend struct TextBlockFigure;

public:
	TextBlock() {}
	~TextBlock();

protected:
	owner_ptr<TextLayout> layout = nullptr;
public:
	void SetText(const TextBlockStyle& style, const u16string& text);
	Rect UpdateLayout(Size size_ref);

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
