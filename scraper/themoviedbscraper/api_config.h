#ifndef __TVSCRAPER_APICONFIG_H
#define __TVSCRAPER_APICONFIG_H

#include <iostream>

#include <string>
#include <unordered_map>
#include <vector>
#include "../../tools/stringhelpers.h"
#include "../../lib/curl.h"

#include "../../epgdconfig.h"

struct sApiData
{
public:
    const std::string apiKey;
    const std::string language{"en"};
    std::vector<std::string> backdrop_sizes;
    std::vector<std::string> logo_sizes;
    std::vector<std::string> poster_sizes;
    std::vector<std::string> profile_sizes;
    std::vector<std::string> still_sizes;
    std::unordered_map<int, std::string> tv_genres;

    const std::string baseUrl = "https://api.themoviedb.org";
    const std::string configurationUrl;
    sApiData(const std::string &apiKey, const std::string &language);
    std::string mediaBaseUrl;
    bool LoadAPIConfig(void);
    bool LoadTvGenres(void);

private:
    const std::string tvGenreUrl;
};

#endif // __TVSCRAPER_APICONFIG_H