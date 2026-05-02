#include "ViewDesign/drawing/text_block.h"


namespace ViewDesign {


TextBlock::~TextBlock() {}

void TextBlock::SetText(const TextBlockStyle& style, const u16string& text) {}

Rect TextBlock::UpdateLayout(Size size_ref) { return region_empty; }

TextBlock::HitTestInfo TextBlock::HitTestPoint(Point point) const { return TextBlock::HitTestInfo{ IndexRange() , region_empty }; }

TextBlock::HitTestInfo TextBlock::HitTestPosition(size_t text_position) const { return TextBlock::HitTestInfo{ IndexRange() , region_empty }; }

std::vector<TextBlock::HitTestInfo> TextBlock::HitTestRange(TextRange range) const { return {}; }


void TextBlockFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign
