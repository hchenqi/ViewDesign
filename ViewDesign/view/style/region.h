#pragma once

#include "ViewDesign/view/style/length.h"
#include "ViewDesign/view/style/position.h"


namespace ViewDesign {


struct RegionStyle {
public:
	LengthStyle width;
	LengthStyle height;
	PositionStyle position;

protected:
	static constexpr Size CalculateSize(LayoutValue width, LayoutValue height, Size size_ref) {
		return Size(width.ToPixel(size_ref.width).value(), height.ToPixel(size_ref.height).value());
	}
public:
	static constexpr std::pair<Size, Size> CalculateMinMaxSize(LengthStyle width, LengthStyle height, Size size_ref) {
		return { CalculateSize(width._min, height._min, size_ref), CalculateSize(width._max, height._max, size_ref) };
	}

private:
	static constexpr bool IsPositionAuto(LayoutValue position) {
		return position.IsAuto() || position.IsCenter();
	}
	static constexpr float CalculatePosition(LayoutValue position_low, LayoutValue position_high, float length, float length_ref) {
		if (IsPositionAuto(position_low)) {
			if (position_low.IsCenter()) {
				position_low = px((length_ref - length) / 2);
			} else if (!position_high.IsAuto()) {
				position_low = px(length_ref - position_high.value() - length);
			}
		}
		return position_low.value();
	}
protected:
	static constexpr Interval CalculateLength(LengthStyle length, LayoutValue position_low, LayoutValue position_high, float length_ref) {
		LayoutValue& length_normal = length._normal.ToPixel(length_ref);
		LayoutValue& length_min = length._min.ToPixel(length_ref);
		LayoutValue& length_max = length._max.ToPixel(length_ref);
		position_low.ToPixel(length_ref);
		position_high.ToPixel(length_ref);
		if (length_normal.IsAuto()) {
			if (IsPositionAuto(position_low) || IsPositionAuto(position_high)) {
				length_normal = length_max;
			} else {
				length_normal = px(length_ref - position_low.value() - position_high.value());
			}
		}
		float length_result = std::clamp(length_normal.value(), length_min.value(), length_max.value());
		return Interval(CalculatePosition(position_low, position_high, length_result, length_ref), length_result);
	}
public:
	static constexpr Rect CalculateRegion(LengthStyle width, LengthStyle height, PositionStyle position, Size size_ref) {
		return Rect(CalculateLength(width, position._left, position._right, size_ref.width), CalculateLength(height, position._top, position._bottom, size_ref.height));
	}
};


} // namespace ViewDesign
