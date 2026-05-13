#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/helper.h"


namespace ViewDesign {

using namespace DirectX;


namespace {

inline ComPtr<D2DBitmap> CreateEmptyD2DBitmap(SizeU size) {
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmap(D2D1::SizeU(size.width, size.height), nullptr, 0, &bitmap_properties, &bitmap);
	return bitmap;
}

} // namespace


void Layer::CreateFramebuffer(SizeU size) {
	DestroyFramebuffer();
	this->size = size;
	framebuffer = CreateEmptyD2DBitmap(size).Detach();
	RegisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(framebuffer));
}

void Layer::DestroyFramebuffer() {
	if (HasFramebuffer()) {
		UnregisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(framebuffer));
		ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(framebuffer));
		size = size_u_empty;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	D2DDeviceContext& device_context = GetD2DDeviceContext();
	device_context.SetTarget(static_cast<ref_ptr<D2DBitmap>>(GetFramebuffer()));
	device_context.PushAxisAlignedClip(AsD2DRect(clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	device_context.Clear(AsD2DColor(color_transparent));
	device_context.SetTransform(AsD2DTransform(offset));
	auto& figure_list = canvas.GetFigureList(); auto& group_list = canvas.GetGroupList();
	for (size_t figure_index = 0, group_index = 0; group_index < group_list.size(); ++group_index) {
		auto& [group_figure_index, group_transform, group_clip_region] = group_list[group_index];
		for (; figure_index < group_figure_index; ++figure_index) {
			auto& [point, figure] = figure_list[figure_index];
			figure->DrawOn(static_cast<RenderTarget&>(device_context), point);
		}
		if (!group_clip_region.IsEmpty()) {
			device_context.SetTransform(AsD2DTransform(group_transform * offset));
			device_context.PushAxisAlignedClip(AsD2DRect(group_clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		} else {
			device_context.PopAxisAlignedClip();
			device_context.SetTransform(AsD2DTransform(group_transform * offset));
		}
	}
	device_context.PopAxisAlignedClip();
	device_context.SetTarget(nullptr);
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		static_cast<ref_ptr<D2DBitmap>>(layer.GetFramebuffer()),
		AsD2DRect(Rect(point, size)),
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
