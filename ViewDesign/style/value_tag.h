#pragma once

#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {


struct ValuePixel {
	float value;

	explicit constexpr ValuePixel(float value) : value(value) {}

	constexpr ValuePixel operator-() const { return ValuePixel(-value); }
};


class ValueTag {
public:
	enum class Type { Pixel, Percent, Center, Auto, _Number };

private:
	Type _type;
	float _value;

public:
	explicit constexpr ValueTag(Type type, float value) : _type(type), _value(value) {}
	constexpr ValueTag(ValuePixel pixel) : ValueTag(Type::Pixel, pixel.value) {}

	constexpr bool IsPixel() const { return _type == Type::Pixel; }
	constexpr bool IsPercent() const { return _type == Type::Percent; }
	constexpr bool IsCenter() const { return _type == Type::Center; }
	constexpr bool IsAuto() const { return _type == Type::Auto; }

	constexpr float value() const { return _value; }

	constexpr ValueTag operator-() const { return ValueTag(_type, -_value); }

	constexpr ValueTag& ConvertToPixel(float length_ref) {
		if (_type == Type::Percent) { _value = length_ref * (_value / 100.0f); _type = Type::Pixel; } return *this;
	}
};


constexpr ValuePixel px(float number) { return ValuePixel(number); }
constexpr ValueTag pct(float number) { return ValueTag(ValueTag::Type::Percent, number); }

/*
constexpr ValuePixel operator""px(long double number) { return px((float)number); }
constexpr ValuePixel operator""px(unsigned long long number) { return px((float)number); }
constexpr ValueTag operator""pct(long double number) { return pct((float)number); }
constexpr ValueTag operator""pct(unsigned long long number) { return pct((float)number); }
*/


constexpr ValueTag length_min_tag = ValueTag(ValueTag::Type::Pixel, length_min);
constexpr ValueTag length_max_tag = ValueTag(ValueTag::Type::Pixel, length_max);
constexpr ValueTag length_auto = ValueTag(ValueTag::Type::Auto, 0.0f);

constexpr ValueTag position_min_tag = ValueTag(ValueTag::Type::Pixel, position_min);
constexpr ValueTag position_max_tag = ValueTag(ValueTag::Type::Pixel, position_max);
constexpr ValueTag position_center = ValueTag(ValueTag::Type::Center, 0.0f);
constexpr ValueTag position_auto = length_auto;


} // namespace ViewDesign
