#pragma once

#include "../common/core.h"


struct IDXGIDevice1;
struct IDXGIAdapter;
struct IDXGIFactory2;


namespace ViewDesign {


IDXGIDevice1& GetDXGIDevice();
IDXGIAdapter& GetDXGIAdapter();
IDXGIFactory2& GetDXGIFactory();


} // namespace ViewDesign
