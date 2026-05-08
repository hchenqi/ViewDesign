#include "ViewDesign/drawing/text_block.h"
#include "ViewDesign/common/text_layout_placeholder.h"


namespace ViewDesign {


TextBlock::~TextBlock() {
	if (layout != nullptr) {
		delete layout;
		layout = nullptr;
	}
}

void TextBlock::SetText(const TextBlockStyle& style, const u16string& text) {
	if (layout != nullptr) {
		delete layout;
	}
	layout = new TextLayout(style.font._size, text.length());
}

Rect TextBlock::UpdateLayout(Size size_ref) {
	return Rect(point_zero, layout->UpdateLayout(size_ref));
}

TextBlock::HitTestPointInfo TextBlock::HitTestPoint(Point point) const {
	return layout->HitTestPoint(point);
}

TextBlock::HitTestPointInfo TextBlock::HitTestPosition(size_t position) const {
	return layout->HitTestPosition(position);
}

TextBlock::HitTestRangeInfo TextBlock::HitTestRange(TextRange range) const {
	return layout->HitTestRange(range);
}


void TextBlockFigure::DrawOn(RenderTarget& target, Point point) const {
	text_block.layout->DrawOn(target, point, font_color);
}


} // namespace ViewDesign
