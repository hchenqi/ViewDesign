#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/win32/d2d_api.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


namespace {

inline owner_ptr<ID2D1Bitmap1> CreateEmptyD2DBitmap(Size size) {
	auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<ID2D1Bitmap1> bitmap;
	hr << GetD2DDeviceContext().CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, &bitmap_properties, &bitmap);
	return bitmap.Detach();
}

} // namespace


void Layer::Create(Size size) {
	texture.Set(static_cast<owner_ptr<TextureResource>>(CreateEmptyD2DBitmap(size)));
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	ID2D1DeviceContext& device_context = GetD2DDeviceContext();
	device_context.SetTarget(texture.GetResource());
	device_context.BeginDraw();
	device_context.SetTransform(AsD2DTransform(offset));
	device_context.PushAxisAlignedClip(AsD2DRect(clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	device_context.Clear(AsD2DColor(color_transparent));
	auto& groups = canvas.GetFigureGroups();
	auto& figures = canvas.GetFigures();
	for (size_t figure_index = 0, group_index = 0; group_index < groups.size(); ++group_index) {
		auto& group = groups[group_index];
		for (; figure_index < group.figure_index; ++figure_index) {
			figures[figure_index].figure->DrawOn(static_cast<RenderTarget&>(device_context), figures[figure_index].offset);
		}
		if (group.IsBegin()) {
			device_context.SetTransform(AsD2DTransform(group.transform * offset));
			device_context.PushAxisAlignedClip(AsD2DRect(group.clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		} else {
			device_context.PopAxisAlignedClip();
			device_context.SetTransform(AsD2DTransform(group.transform * offset));
		}
	}
	device_context.PopAxisAlignedClip();
	hr << device_context.EndDraw();
	device_context.SetTarget(nullptr);
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		layer.GetTexture().GetResource(),
		AsD2DRect(Rect(point, size)),
		OpacityAsFloat(opacity),
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
