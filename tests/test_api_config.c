#include "../scraper/themoviedbscraper/api_config.h"

const char* logPrefix = "test_api_config";


int main() {
    sApiData apiData("abb01b5a277b9c2c60ec0302d83c5ee9","de");
    apiData.LoadAPIConfig ();

    std::cout << "TV Genres: ";
    for (const auto &e: apiData.tv_genres) {
        std::cout << e.first << ": " << e.second << ", ";
    }
    std::cout << "\n";

    std::cout << "image base_url: " << apiData.mediaBaseUrl << "\n";
    std::cout << "backdrop sizes: ";
    for (const auto &size: apiData.backdrop_sizes)
        std::cout << size << ", ";
    std::cout << "\n" << "logo_sizes: ";
    for (const auto &size: apiData.logo_sizes)
        std::cout << size << ", ";
    std::cout << "\n" << "poster_sizes: ";
    for (const auto &size: apiData.poster_sizes)
        std::cout << size << ", ";
    std::cout << "\n" << "profile_sizes: ";
    for (const auto &size: apiData.profile_sizes)
        std::cout << size << ", ";
    std::cout << "\n" << "still_sizes: ";
    for (const auto &size: apiData.still_sizes)
        std::cout << size << ", ";
    std::cout << "\n";

    // json_t* root_data;
    // {
    //     cJsonNode child;
    //     {
    //         auto root = cJsonNode("{\"foo\":\"bar\"}");
    //         root_data = root.object();
    //         std::cerr << "root_data is an Object? " << ((json_typeof(root_data) == JSON_OBJECT) ? "yes" : "no") << "\n";
    //         child = root.nodeByName("foo");
    //     }
    //     std::cerr << "value of foo: " << child.stringValue() << "\n";
    //     std::cerr << "root_data is an Object? " << ((json_typeof(root_data) == JSON_OBJECT) ? "yes" : "no") << "\n";

    // }
    // std::cerr << "root_data is an Object? " << ((json_typeof(root_data) == JSON_OBJECT) ? "yes" : "no") << "\n";
    // std::cerr << "root_data is JSON_NULL? " << ((json_typeof(root_data) == JSON_NULL) ? "yes" : "no") << "\n";
    // std::cerr << "root_data is nullptr? " << ((root_data) ? "yes" : "no") << "\n";
}