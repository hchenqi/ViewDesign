#include "ViewDesign/drawing/image.h"
#include "ViewDesign/platform/win32/d2d_api.h"
#include "ViewDesign/platform/win32/wic_api.h"
#include "ViewDesign/platform/win32/directx_helper.h"

#include <stdexcept>


namespace ViewDesign {

using namespace Win32;


namespace {


ComPtr<IWICFormatConverter> LoadFromDecoder(ComPtr<IWICBitmapDecoder> decoder) {
	ComPtr<IWICFormatConverter> converter;
	ComPtr<IWICBitmapFrameDecode> source;
	hr << GetWICFactory().CreateFormatConverter(&converter);
	hr << decoder->GetFrame(0, &source);
	hr << converter->Initialize(
		source.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.0f,
		WICBitmapPaletteTypeMedianCut
	);
	return converter;
}

ComPtr<IWICFormatConverter> LoadImageFromFile(std::wstring file_name) {
	try {
		ComPtr<IWICBitmapDecoder> decoder;
		hr << GetWICFactory().CreateDecoderFromFilename(
			file_name.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);
		return LoadFromDecoder(decoder);
	} catch (std::runtime_error&) {
		throw std::invalid_argument("invalid image file");
	}
}

ComPtr<IWICFormatConverter> LoadImageFromMemory(void* address, size_t size) {
	try {
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		hr << GetWICFactory().CreateStream(&stream);
		hr << stream->InitializeFromMemory(reinterpret_cast<BYTE*>(address), (DWORD)size);
		hr << GetWICFactory().CreateDecoderFromStream(
			stream.Get(),
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
		return LoadFromDecoder(decoder);
	} catch (std::runtime_error&) {
		throw std::invalid_argument("invalid image address");
	}
}


inline Size GetImageSize(ImageSource& source) {
	uint width, height;
	hr << source.GetSize(&width, &height);
	return Size((float)width, (float)height);
}

inline ComPtr<D2DBitmap> CreateD2DBitmapFromWicBitmap(IWICFormatConverter& converter) {
	ComPtr<D2DBitmap> bitmap;
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	hr << GetD2DDeviceContext().CreateBitmapFromWicBitmap(&converter, &bitmap_properties, &bitmap);
	return bitmap;
}


} // namespace


Image::Image(std::wstring file_name) : source(static_cast<ImageSource*>(LoadImageFromFile(file_name).Detach())), size(GetImageSize(*source)) {}

Image::Image(void* address, size_t size) : source(static_cast<ImageSource*>(LoadImageFromMemory(address, size).Detach())), size(GetImageSize(*source)) {}

Image::~Image() { SafeRelease(&source); }

void Image::CreateBitmap() const {
	if (bitmap.IsEmpty()) {
		bitmap.Set(static_cast<BitmapResource*>(CreateD2DBitmapFromWicBitmap(*source).Detach()));
	}
}


void ImageFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		image.bitmap.Get(),
		AsD2DRect(Rect(point, region.size)),
		OpacityAsFloat(opacity),
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
