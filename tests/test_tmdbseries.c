#include <iostream>
#include <vector>
#include <memory>
#include <jansson.h>
#include "../tools/stringhelpers.h"
#include "../scraper/themoviedbscraper/api_config.h"
#include "../scraper/themoviedbscraper/tmdbactor.h"
#include "../scraper/themoviedbscraper/tmdbseries.h"

const char* logPrefix = "test_tmdbseries.c";

auto config = std::make_shared<sApiData>("abb01b5a277b9c2c60ec0302d83c5ee9", "de");

int main() {
    config->LoadAPIConfig(); // load current configurationd data from server
    // auto series = cTMDBSeries(data, config);
    auto series = cTMDBSeries(1399, config);
    series.ReadSeries();
    series.ReadActors();
    // series.ReadMedia();
    series.Dump(2);
    return 0;
}