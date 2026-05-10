#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/helper.h"
#include "ViewDesign/platform/directx/resource.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct DesktopPrivateAccess : Desktop {
	using Desktop::RecreateWindowLayer;
};

using namespace DirectX;


namespace {

inline ComPtr<D2DBitmap> CreateEmptyD2DBitmap(Size size) {
	auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, &bitmap_properties, &bitmap);
	return bitmap;
}

} // namespace


void Layer::CreateTexture(Size size) {
	DestroyTexture();
	this->size = size;
	texture = CreateEmptyD2DBitmap(size).Detach();
	RegisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
}

void Layer::DestroyTexture() {
	if (!IsEmpty()) {
		UnregisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
		ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
		size = size_empty;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	ID2D1DeviceContext& device_context = GetD2DDeviceContext();
	device_context.SetTarget(static_cast<ref_ptr<D2DBitmap>>(GetTexture()));
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
	try {
		hr << device_context.EndDraw();
	} catch (std::runtime_error&) {
		RecreateResource();
		static_cast<DesktopPrivateAccess&>(desktop.Get()).RecreateWindowLayer();
	}
	device_context.SetTarget(nullptr);
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		static_cast<ref_ptr<D2DBitmap>>(layer.GetTexture()),
		AsD2DRect(Rect(point, size)),
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
