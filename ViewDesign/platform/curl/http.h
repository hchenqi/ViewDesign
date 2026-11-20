#pragma once

#include "ViewDesign/common/unicode.h"

#include <vector>


namespace ViewDesign {

namespace Curl {


std::vector<std::byte> DownloadBytes(const u8string& url);


} // namespace Curl

} // namespace ViewDesign
