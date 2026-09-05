#include "ViewDesign/platform/curl/http.h"

#include <curl/curl.h>

#include <stdexcept>


namespace ViewDesign {

namespace Curl {

namespace {

size_t WriteCallback(char* data, size_t size, size_t count, void* userdata) {
	auto& buffer = *static_cast<std::vector<unsigned char>*>(userdata);
	buffer.insert(buffer.end(), data, data + size * count);
	return size * count;
}

} // namespace


std::vector<unsigned char> DownloadBytes(const std::string& url) {
	CURL* curl = curl_easy_init();
	if (curl == nullptr) {
		throw std::runtime_error("Curl: failed to initialize");
	}

	std::vector<unsigned char> buffer;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "ViewDesign");

	CURLcode result = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (result != CURLE_OK || buffer.empty()) {
		throw std::runtime_error("Curl: failed to download image from URL");
	}

	return buffer;
}


} // namespace Curl

} // namespace ViewDesign
