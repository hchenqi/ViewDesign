#include "ViewDesign/platform/win32/icon.h"

#include <wincodec.h>
#include <shlwapi.h>
#include <wrl/client.h>

#include <stdexcept>
#include <vector>


namespace ViewDesign {

namespace Win32 {

namespace {

using Microsoft::WRL::ComPtr;

constexpr int icon_size = 32;

IWICImagingFactory& GetWICFactory() {
	static ComPtr<IWICImagingFactory> factory = [] {
		CoInitialize(nullptr);
		ComPtr<IWICImagingFactory> factory;
		CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		return factory;
	}();
	return *factory.Get();
}

} // namespace


HICON CreateIconFromBytes(const void* buffer, size_t size) {
	ComPtr<IStream> stream;
	stream.Attach(SHCreateMemStream(reinterpret_cast<const BYTE*>(buffer), (UINT)size));
	if (!stream) {
		throw std::runtime_error("Win32: failed to create memory stream for icon");
	}

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(GetWICFactory().CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder))) {
		throw std::runtime_error("Win32: failed to decode icon image");
	}

	ComPtr<IWICBitmapFrameDecode> frame;
	if (FAILED(decoder->GetFrame(0, &frame))) {
		throw std::runtime_error("Win32: failed to read icon image frame");
	}

	ComPtr<IWICBitmapScaler> scaler;
	GetWICFactory().CreateBitmapScaler(&scaler);
	scaler->Initialize(frame.Get(), icon_size, icon_size, WICBitmapInterpolationModeFant);

	ComPtr<IWICFormatConverter> converter;
	GetWICFactory().CreateFormatConverter(&converter);
	converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

	std::vector<BYTE> pixels(icon_size * icon_size * 4);
	if (FAILED(converter->CopyPixels(nullptr, icon_size * 4, (UINT)pixels.size(), pixels.data()))) {
		throw std::runtime_error("Win32: failed to copy icon pixels");
	}

	HBITMAP color = CreateBitmap(icon_size, icon_size, 1, 32, pixels.data());

	std::vector<BYTE> mask(icon_size * icon_size * 4, 0);
	HBITMAP and_mask = CreateBitmap(icon_size, icon_size, 1, 32, mask.data());

	ICONINFO icon_info = {};
	icon_info.fIcon = TRUE;
	icon_info.hbmColor = color;
	icon_info.hbmMask = and_mask;

	HICON icon = CreateIconIndirect(&icon_info);

	DeleteObject(color);
	DeleteObject(and_mask);

	if (icon == nullptr) {
		throw std::runtime_error("Win32: failed to create icon");
	}

	return icon;
}


} // namespace Win32

} // namespace ViewDesign
