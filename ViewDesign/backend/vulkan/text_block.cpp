#include "ViewDesign/drawing/text_block.h"
#include "ViewDesign/common/text_layout_placeholder.h"


namespace ViewDesign {

namespace {

inline ref_ptr<TextLayoutPlaceholder> AsTextLayoutPlaceholder(Handle layout) {
	return static_cast<ref_ptr<TextLayoutPlaceholder>>(layout);
}

} // namespace


TextBlock::TextBlock() : layout(new TextLayoutPlaceholder()) {}

TextBlock::~TextBlock() { delete static_cast<owner_ptr<TextLayoutPlaceholder>>(layout); }

void TextBlock::SetText(const TextBlockStyle& style, const u16string& text) {
	AsTextLayoutPlaceholder(layout)->SetStyle(style);
	AsTextLayoutPlaceholder(layout)->SetText(text);	
}

Rect TextBlock::UpdateLayout(Size size_ref) {
	return Rect(point_zero, AsTextLayoutPlaceholder(layout)->UpdateLayout(size_ref));
}

TextBlock::HitTestPointInfo TextBlock::HitTestPoint(Point point) const {
	return AsTextLayoutPlaceholder(layout)->HitTestPoint(point);
}

TextBlock::HitTestPointInfo TextBlock::HitTestPosition(size_t position) const {
	return AsTextLayoutPlaceholder(layout)->HitTestPosition(position);
}

TextBlock::HitTestRangeInfo TextBlock::HitTestRange(TextRange range) const {
	return AsTextLayoutPlaceholder(layout)->HitTestRange(range);
}


void TextBlockFigure::DrawOn(RenderTarget& target, Point point) const {
	AsTextLayoutPlaceholder(text_block.layout)->DrawOn(target, point, font_color);
}


} // namespace ViewDesign
