#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/helper.h"


namespace ViewDesign {

using namespace DirectX;


namespace {

inline ComPtr<D2DBitmap> CreateD2DBitmap(const PixelBuffer& pixel_buffer) {
	auto [width, height] = pixel_buffer.Size();
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmap(D2D1::SizeU(width, height), pixel_buffer.PixelData(), width * sizeof(Color), &bitmap_properties, &bitmap);
	return bitmap;
}

} // namespace


void Bitmap::CreateTexture() const {
	if (texture == nullptr) {
		texture = CreateD2DBitmap(pixel_buffer).Detach();
		RegisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
	}
}

void Bitmap::DestroyTexture() const {
	if (texture != nullptr) {
		UnregisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
		ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
	}
}


void BitmapFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		static_cast<ref_ptr<D2DBitmap>>(bitmap.GetTexture()),
		AsD2DRect(Rect(point, region.size)),
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
