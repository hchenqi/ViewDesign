#pragma once

#include "ViewDesign/view/figure/text_block.h"
#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/geometry/helper.h"

#include <cmath>


namespace ViewDesign {


struct TextLayoutPlaceholder {
	// style
	static constexpr Size symbol_size_default = Size(0.5f, 1.0f);
	static constexpr float symbol_gap = 1.0f;
	static constexpr float row_gap = 5.0f;

	Size symbol_size;

	void SetStyle(const TextStyle& style) {
		symbol_size = symbol_size_default * Scale(style.font._size);
	}

	// text
	size_t length;

	void SetText(const u16string& text) {
		length = text.length();
	}

	// layout
	size_t symbol_number;
	size_t row_number;
	Size size;

	Rect GetSymbolRect(size_t row_index, size_t symbol_index) const {
		return Rect(Point(symbol_index * (symbol_size.width + symbol_gap), row_index * (symbol_size.height + row_gap)), symbol_size);
	}

	Rect GetSymbolRect(size_t index) const {
		return GetSymbolRect(index / symbol_number, index % symbol_number);
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

	TextBlock::HitTestPointInfo HitTestPosition(TextRange position) const {
		if (position.end() >= length) {
			position = length == 0 ? TextRange(0, 0) : TextRange(length - 1, 1);
		}
		if (position.length() == 0) {
			return std::make_pair(TextRange(position.end(), 0), GetSymbolRect(position.end()));
		} else {
			return std::make_pair(TextRange(position.end() - 1, 1), GetSymbolRect(position.end() - 1));
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
			result.emplace_back(std::make_pair(TextRange(symbol_index_begin, symbol_number - symbol_index_begin), GetSymbolRangeRect(row_index_begin, symbol_index_begin, symbol_number - 1)));
			for (size_t row_index = row_index_begin + 1; row_index <= row_index_end - 1; ++row_index) {
				result.emplace_back(std::make_pair(TextRange(row_index * symbol_number, symbol_number), GetSymbolRangeRect(row_index, 0, symbol_number - 1)));
			}
			result.emplace_back(std::make_pair(TextRange(row_index_end * symbol_number, symbol_index_end + 1), GetSymbolRangeRect(row_index_end, 0, symbol_index_end)));
		}
		return result;
	}

	void DrawOn(RenderTarget& target, Point point, Color font_color) {
		for (size_t index = 0; index < length; ++index) {
			Rect rect = Extend(GetSymbolRect(index), -1.0f);
			Rectangle rectangle(rect.size, font_color);
			rectangle.DrawOn(target, point + (rect.point - point_zero));
		}
	}
};


} // namespace ViewDesign
