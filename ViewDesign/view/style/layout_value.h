#pragma once

#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {


struct PixelValue {
	float value;

	explicit constexpr PixelValue(float value) : value(value) {}

	constexpr PixelValue operator-() const { return PixelValue(-value); }
};


class LayoutValue {
public:
	enum class Type { Pixel, Percent, Center, Auto };

private:
	Type _type;
	float _value;

public:
	explicit constexpr LayoutValue(Type type, float value) : _type(type), _value(value) {}
	constexpr LayoutValue(PixelValue pixel) : LayoutValue(Type::Pixel, pixel.value) {}

	constexpr bool IsPixel() const { return _type == Type::Pixel; }
	constexpr bool IsPercent() const { return _type == Type::Percent; }
	constexpr bool IsCenter() const { return _type == Type::Center; }
	constexpr bool IsAuto() const { return _type == Type::Auto; }

	constexpr float value() const { return _value; }

	constexpr LayoutValue operator-() const { return LayoutValue(_type, -_value); }

	constexpr LayoutValue& ToPixel(float length_ref) {
		if (_type == Type::Percent) {
			_value = length_ref * (_value / 100.0f);
			_type = Type::Pixel;
		}
		return *this;
	}
};


constexpr PixelValue px(float number) { return PixelValue(number); }
constexpr LayoutValue pct(float number) { return LayoutValue(LayoutValue::Type::Percent, number); }

/*
constexpr PixelValue operator""px(long double number) { return px((float)number); }
constexpr PixelValue operator""px(unsigned long long number) { return px((float)number); }
constexpr LayoutValue operator""pct(long double number) { return pct((float)number); }
constexpr LayoutValue operator""pct(unsigned long long number) { return pct((float)number); }
*/


constexpr PixelValue length_zero_value = PixelValue(length_zero);
constexpr PixelValue length_infinite_value = PixelValue(length_infinite);
constexpr LayoutValue length_auto = LayoutValue(LayoutValue::Type::Auto, 0.0f);

constexpr PixelValue position_max_value = PixelValue(position_max);
constexpr PixelValue position_min_value = PixelValue(position_min);
constexpr LayoutValue position_center = LayoutValue(LayoutValue::Type::Center, 0.0f);
constexpr LayoutValue position_auto = LayoutValue(LayoutValue::Type::Auto, 0.0f);


} // namespace ViewDesign
