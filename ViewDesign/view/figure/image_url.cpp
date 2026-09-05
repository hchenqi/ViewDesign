#include "ViewDesign/view/figure/image.h"

#ifdef VIEWDESIGN_PLATFORM_CURL
#include <ViewDesign/platform/curl/http.h>
#endif


namespace ViewDesign {


#ifdef VIEWDESIGN_PLATFORM_CURL

Image::Image(const std::string& url) : Image(Curl::DownloadBytes(url)) {}

#else

Image::Image(const std::string& url) : source(nullptr) {
	throw std::runtime_error("Image: loading from a URL requires the Curl platform module");
}

#endif


} // namespace ViewDesign
