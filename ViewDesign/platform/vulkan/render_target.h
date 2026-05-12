#pragma once

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {


struct RenderTarget : public vk::raii::CommandBuffer {};


} // namespace ViewDesign
