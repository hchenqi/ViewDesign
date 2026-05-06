#pragma once

#include "ViewDesign/geometry/helper.h"
#include "ViewDesign/drawing/text_block.h"
#include "ViewDesign/drawing/shape.h"


namespace ViewDesign {


struct TextLayout {
	TextLayout(float font_size, size_t length) : symbol_size(symbol_size_default * Scale(font_size)), length(length) {}

	// style
	static constexpr Size symbol_size_default = Size(0.5f, 1.0f);
	static constexpr float symbol_gap = 1.0f;
	static constexpr float row_gap = 5.0f;

	Size symbol_size;

	// text
	size_t length;

	// layout
	size_t symbol_number;
	size_t row_number;
	Size size;

	Rect GetSymbolRect(size_t row_index, size_t symbol_index) const {
		return Rect(Point(symbol_index * (symbol_size.width + symbol_gap), row_index * (symbol_size.height + row_gap)), symbol_size);
	}

	Rect GetSymbolRangeRect(size_t row_index, size_t symbol_index_begin, size_t symbol_index_end) const {
		return Rect(Point(symbol_index_begin * (symbol_size.width + symbol_gap), row_index * (symbol_size.height + row_gap)), Size((symbol_size.width + symbol_gap) * (symbol_index_end - symbol_index_begin + 1) - symbol_gap, symbol_size.height));
	}

	Size UpdateLayout(Size size_ref) {
		symbol_number = std::max((size_t)1, (size_t)floorf((size_ref.width + symbol_gap) / (symbol_size.width + symbol_gap)));
		row_number = std::max((size_t)1, (length + symbol_number - 1) / symbol_number);
		size = Size((row_number == 1 ? length : symbol_number) * (symbol_size.width + symbol_gap) - symbol_gap, row_number * (symbol_size.height + row_gap) - row_gap);
		return size;
	}

	TextBlock::HitTestPointInfo HitTestPoint(Point point) const {
		size_t symbol_index = std::max(0, (int)floorf(point.x / (symbol_size.width + symbol_gap)));
		size_t row_index = std::max(0, (int)floorf(point.y / (symbol_size.height + row_gap)));
		row_index = std::min(row_index, row_number - 1);
		symbol_index = std::min(symbol_index, row_index == row_number - 1 ? length == 0 ? 0 : length - row_index * symbol_number - 1 : symbol_number - 1);
		return std::make_pair(TextRange(row_index * symbol_number + symbol_index, length == 0 ? 0 : point.x >= GetSymbolRect(row_index, symbol_index).Center().x ? 1 : 0), GetSymbolRect(row_index, symbol_index));
	}

	TextBlock::HitTestPointInfo HitTestPosition(size_t position) const {
		if (length == 0) {
			return std::make_pair(TextRange(0, 0), GetSymbolRect(0, 0));
		} else {
			if (position >= length) {
				position = length - 1;
				return std::make_pair(TextRange(position, 1), GetSymbolRect(position / symbol_number, position % symbol_number));
			} else {
				return std::make_pair(TextRange(position, 0), GetSymbolRect(position / symbol_number, position % symbol_number));
			}
		}
	}

	TextBlock::HitTestRangeInfo HitTestRange(TextRange range) const {
		if (range.begin() >= length) { return {}; }
		if (range.end() > length) { range._length = length - range.begin(); }
		if (range.length() == 0) { return {}; }
		TextBlock::HitTestRangeInfo result;
		size_t position_begin = range.begin(), position_end = range.end() - 1;
		size_t row_index_begin = position_begin / symbol_number, symbol_index_begin = position_begin % symbol_number;
		size_t row_index_end = position_end / symbol_number, symbol_index_end = position_end % symbol_number;
		if (row_index_begin == row_index_end) {
			result.emplace_back(std::make_pair(TextRange(symbol_index_begin, symbol_index_end - symbol_index_begin + 1), GetSymbolRangeRect(row_index_begin, symbol_index_begin, symbol_index_end)));
		} else {
			result.emplace_back(std::make_pair(TextRange(symbol_index_begin, (row_index_begin + 1) * symbol_number - symbol_index_begin), GetSymbolRangeRect(row_index_begin, symbol_index_begin, (row_index_begin + 1) * symbol_number - 1)));
			for (size_t row_index = row_index_begin + 1; row_index <= row_index_end - 1; ++row_index) {
				result.emplace_back(std::make_pair(TextRange(row_index * symbol_number, symbol_number), GetSymbolRangeRect(row_index, row_index * symbol_number, (row_index + 1) * symbol_number - 1)));
			}
			result.emplace_back(std::make_pair(TextRange(row_index_end * symbol_number, symbol_index_end - row_index_end * symbol_number + 1), GetSymbolRangeRect(row_index_end, row_index_end * symbol_number, symbol_index_end - row_index_end * symbol_number)));
		}
		return result;
	}

	void DrawOn(RenderTarget& target, Point point, Color font_color) {
		for (size_t i = 0; i < length; ++i) {
			Rect rect = Extend(GetSymbolRect(i / symbol_number, i % symbol_number), -1.0f);
			Rectangle rectangle(rect.size, font_color);
			rectangle.DrawOn(target, point + (rect.point - point_zero));
		}
	}
};


} // namespace ViewDesign
