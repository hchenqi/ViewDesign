#pragma once


namespace ViewDesign {

namespace Win32 {

struct DXGIDevice;
struct DXGIAdapter;
struct DXGIFactory;


DXGIDevice& GetDXGIDevice();
DXGIAdapter& GetDXGIAdapter();
DXGIFactory& GetDXGIFactory();


} // namespace Win32

} // namespace ViewDesign
