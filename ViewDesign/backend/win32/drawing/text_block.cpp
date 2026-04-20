#include "ViewDesign/drawing/text_block.h"
#include "ViewDesign/backend/win32/d2d_api.h"
#include "ViewDesign/backend/win32/dwrite_api.h"
#include "ViewDesign/backend/win32/directx_helper.h"


namespace ViewDesign {


TextBlock::~TextBlock() {
	SafeRelease(&layout);
}

void TextBlock::SetText(const TextBlockStyle& style, const std::wstring& text) {
	SafeRelease(&layout);

	// font format
	ComPtr<IDWriteTextFormat> format;
	hr << GetDWriteFactory().CreateTextFormat(
		L"",
		nullptr,
		static_cast<DWRITE_FONT_WEIGHT>(style.font._weight),
		static_cast<DWRITE_FONT_STYLE>(style.font._style),
		static_cast<DWRITE_FONT_STRETCH>(style.font._stretch),
		style.font._size,
		style.font._locale.c_str(),
		&format
	);

	// font layout
	ComPtr<IDWriteTextLayout> layout_0;
	ComPtr<IDWriteTextLayout4> layout_4;
	hr << GetDWriteFactory().CreateTextLayout(
		text.c_str(), (uint)text.length(),
		format.Get(), 0, 0, &layout_0
	);
	hr << layout_0.As(&layout_4);
	layout = static_cast<TextLayout*>(layout_4.Detach());

	// font fallback
	ComPtr<IDWriteFontFallbackBuilder> font_fallback_builder;
	GetDWriteFactory().CreateFontFallbackBuilder(&font_fallback_builder);

	DWRITE_UNICODE_RANGE range = { 0, (uint)-1 };
	std::vector<const wchar*> family_list; family_list.reserve(style.font._family_list.size());
	for (auto& str : style.font._family_list) { family_list.push_back(str.c_str()); }
	font_fallback_builder->AddMapping(&range, 1, family_list.data(), (uint)family_list.size());

	ComPtr<IDWriteFontFallback> system_font_fallback;
	GetDWriteFactory().GetSystemFontFallback(&system_font_fallback);
	font_fallback_builder->AddMappings(system_font_fallback.Get());

	ComPtr<IDWriteFontFallback> font_fallback;
	font_fallback_builder->CreateFontFallback(&font_fallback);
	layout->SetFontFallback(font_fallback.Get());

	// paragraph style
	ValueTag line_spacing = style.paragraph._line_spacing;
	ValueTag baseline_spacing = style.paragraph._baseline_spacing;
	ValueTag tab_size = style.paragraph._tab_size;
	layout->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(style.paragraph._text_align));
	layout->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(style.paragraph._paragraph_align));
	layout->SetFlowDirection(static_cast<DWRITE_FLOW_DIRECTION>(style.paragraph._flow_direction));
	layout->SetReadingDirection(static_cast<DWRITE_READING_DIRECTION>(style.paragraph._read_direction));
	layout->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(style.paragraph._word_wrap));
	if (line_spacing.IsPixel()) {
		layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, line_spacing.value(), baseline_spacing.ConvertToPixel(line_spacing.value()).value());
	} else if (line_spacing.IsPercent()) {
		line_spacing.ConvertToPixel(1.0f);
		layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, line_spacing.value(), baseline_spacing.ConvertToPixel(line_spacing.value()).value());
	} else {
		layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 0.0f, 0.0f);
	}
	layout->SetIncrementalTabStop(tab_size.ConvertToPixel(style.font._size).value());
}

Rect TextBlock::UpdateLayout(Size size_ref) {
	layout->SetMaxWidth(size_ref.width); layout->SetMaxHeight(size_ref.height);
	DWRITE_TEXT_METRICS metrics; layout->GetMetrics(&metrics);
	return Rect(metrics.left, metrics.top, metrics.widthIncludingTrailingWhitespace, metrics.height);
}


inline TextBlock::HitTestInfo HitTestMetricsToInfo(DWRITE_HIT_TEST_METRICS& metrics) {
	return TextBlock::HitTestInfo{
		TextRange(metrics.textPosition, metrics.length),
		Rect(metrics.left, metrics.top, metrics.width, metrics.height)
	};
}

TextBlock::HitTestInfo TextBlock::HitTestPoint(Point point) const {
	BOOL isTrailingHit; BOOL isInside; DWRITE_HIT_TEST_METRICS metrics;
	layout->HitTestPoint(point.x, point.y, &isTrailingHit, &isInside, &metrics);
	if (isTrailingHit) { metrics.textPosition += metrics.length; metrics.left += metrics.width; }
	return HitTestMetricsToInfo(metrics);
}

TextBlock::HitTestInfo TextBlock::HitTestPosition(size_t text_position) const {
	FLOAT x, y; DWRITE_HIT_TEST_METRICS metrics;
	layout->HitTestTextPosition((uint)text_position, false, &x, &y, &metrics);
	return HitTestMetricsToInfo(metrics);
}

std::vector<TextBlock::HitTestInfo> TextBlock::HitTestRange(TextRange range) const {
	UINT32 line_cnt; layout->GetLineMetrics((DWRITE_LINE_METRICS1*)nullptr, 0, &line_cnt);
	UINT32 actual_size = line_cnt;
	std::vector<DWRITE_HIT_TEST_METRICS> metrics;
	do {
		metrics.resize(actual_size);
		layout->HitTestTextRange((uint)range.begin(), (uint)range.length(), 0, 0, metrics.data(), (uint)metrics.size(), &actual_size);
	} while (actual_size > metrics.size());
	metrics.resize(actual_size);
	std::vector<HitTestInfo> geometry_regions(metrics.size());
	for (size_t i = 0; i < geometry_regions.size(); ++i) {
		geometry_regions[i] = HitTestMetricsToInfo(metrics[i]);
		if (geometry_regions[i].region.size.width < 5.0f) { geometry_regions[i].region.size.width = 5.0f; }
	}
	return geometry_regions;
}


void TextBlockFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawTextLayout(
		AsD2DPoint(point),
		text_block.layout,
		&GetD2DSolidColorBrush(font_color),
		D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
	);
}


} // namespace ViewDesign
