#pragma once

#include "ViewDesign/style/length_style.h"
#include "ViewDesign/geometry/clamp.h"


namespace ViewDesign {


struct LengthStyleHelper {
public:
	static constexpr Size CalculateSize(ValueTag width, ValueTag height, Size size_ref) {
		return Size(width.ConvertToPixel(size_ref.width).value(), height.ConvertToPixel(size_ref.height).value());
	}
	static constexpr std::pair<Size, Size> CalculateMinMaxSize(LengthStyle width, LengthStyle height, Size size_ref) {
		return { CalculateSize(width._min, height._min, size_ref), CalculateSize(width._max, height._max, size_ref) };
	}
public:
	static constexpr Rect MakeRectFromInterval(Interval horizontal, Interval vertical) {
		return Rect(horizontal.begin, vertical.begin, horizontal.length, vertical.length);
	}
	static constexpr bool IsPositionAuto(ValueTag position) { return position.IsAuto() || position.IsCenter(); }
	static constexpr float CalculatePosition(ValueTag position_low, ValueTag position_high, float length, float length_ref) {
		if (IsPositionAuto(position_low)) {
			if (position_low.IsCenter()) {
				position_low = px((length_ref - length) / 2);
			} else if (!position_high.IsAuto()) {
				position_low = px(length_ref - position_high.value() - length);
			}
		}
		return position_low.value();
	}
	static constexpr Interval CalculateLength(LengthStyle length, ValueTag position_low, ValueTag position_high, float length_ref) {
		if (length_ref == 0) { return Interval(); }
		ValueTag& length_normal = length._normal.ConvertToPixel(length_ref);
		ValueTag& length_min = length._min.ConvertToPixel(length_ref);
		ValueTag& length_max = length._max.ConvertToPixel(length_ref);
		position_low.ConvertToPixel(length_ref);
		position_high.ConvertToPixel(length_ref);
		if (length_normal.IsAuto()) {
			if (IsPositionAuto(position_low) || IsPositionAuto(position_high)) {
				length_normal = length_max;
			} else {
				length_normal = px(length_ref - position_low.value() - position_high.value());
			}
		}
		float length_result = clamp(length_normal.value(), length_min.value(), length_max.value());
		return Interval(CalculatePosition(position_low, position_high, length_result, length_ref), length_result);
	}
public:
	static constexpr Rect CalculateRegion(LengthStyle width, LengthStyle height, PositionStyle position, Size size_ref) {
		return MakeRectFromInterval(
			CalculateLength(width, position._left, position._right, size_ref.width),
			CalculateLength(height, position._top, position._bottom, size_ref.height)
		);
	}
};


} // namespace ViewDesign
