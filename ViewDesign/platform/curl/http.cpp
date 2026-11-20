#include "ViewDesign/platform/curl/http.h"

#include <curl/curl.h>

#include <stdexcept>


namespace ViewDesign {

namespace Curl {

namespace {

size_t WriteCallback(std::byte* data, size_t size, size_t count, void* userdata) {
	auto& buffer = *static_cast<std::vector<std::byte>*>(userdata);
	buffer.insert(buffer.end(), data, data + size * count);
	return size * count;
}

} // namespace


std::vector<std::byte> DownloadBytes(const u8string& url) {
	std::vector<std::byte> buffer;

	CURL* curl = curl_easy_init();
	if (curl == nullptr) {
		throw std::runtime_error("Curl: failed to initialize");
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "ViewDesign");

	CURLcode result = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	if (result != CURLE_OK) {
		throw std::runtime_error("Curl: failed to download from URL");
	}

	return buffer;
}


} // namespace Curl

} // namespace ViewDesign
