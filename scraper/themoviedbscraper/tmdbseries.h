#ifndef __TVSCRAPER_TMDBSERIES_H
#define __TVSCRAPER_TMDBSERIES_H

#include <string>
#include <sstream>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <jansson.h>

#include "../../lib/curl.h"
#include "../../tools/fuzzy.h"
#include "../../tools/stringhelpers.h"

#include "tmdbactor.h"
#include "tmdbepisode.h"
#include "tmdbmedia.h"
#include "api_config.h"

using string = std::string;

class cTMDBSeries {
private:
    const std::shared_ptr<sApiData> _config;
    vector<std::unique_ptr<cTMDBEpisode>> episodes;
    vector<std::unique_ptr<cTMDBActor>> actors;
    vector<std::unique_ptr<cTMDBFanart>> fanarts;
    vector<std::unique_ptr<cTMDBPoster>> posters;
    vector<std::unique_ptr<cTMDBSeasonPoster>> seasonPosters;
    vector<std::unique_ptr<cTMDBBanner>> banners;
    std::unordered_map<std::string, int> language_order { {"en", 1}, {"", 2} }; // preferred sort order
    int currentEpisode = 0;
    int currentActor = 0;
    int currentFanart = 0;
    int currentPoster = 0;
    int currentSeasonPoster = 0;
    int currentBanner = 0;
    // void ParseXML(string xml);
    void ReadSeriesData(const cJsonNode &data);
    void ParseSeason(const cJsonNode &jSeason);
    void ParseEpisode(const cJsonNode &episodeData);
    cJsonNode LoadSeason(const int seasonNumber);
    bool LoadGenres(void);
public:
    cTMDBSeries(const int ID, const std::shared_ptr<sApiData> config);
    cTMDBSeries(const string &seriesName, const std::shared_ptr<sApiData> config);
    cTMDBSeries(const cJsonNode &data, const std::shared_ptr<sApiData> config);
    cTMDBSeries() = default;
    virtual ~cTMDBSeries() = default;
    cTMDBSeries(const cTMDBSeries&) = delete;
    cTMDBSeries(cTMDBSeries&&) = default;
    // void SetApiKey(string apiKey) { this->apiKey = apiKey; };
    // void SetBaseUrl(string baseUrl) { this->baseUrl = baseUrl; };
    bool ReadSeries(void);
    void ReadMedia(void);
    void ReadMedia(const cJsonNode &data);
    void ReadActors(void);
    
    template <class T>
    void SortMedia(std::vector<std::unique_ptr<T>> &vMedia);

    void SortPosters(void);
    void SortFanart(void);
    const std::vector<std::unique_ptr<cTMDBEpisode>> &GetEpisode(void);
    const std::vector<std::unique_ptr<cTMDBActor>> &GetActor(void);
    const std::vector<std::unique_ptr<cTMDBFanart>> &GetFanart(void);
    const std::vector<std::unique_ptr<cTMDBPoster>> &GetPoster(void);
    const std::vector<std::unique_ptr<cTMDBBanner>> &GetBanner(void);
    const std::vector<std::unique_ptr<cTMDBSeasonPoster>> &GetSeasonPoster(void);
    const std::unique_ptr<cTMDBSeasonPoster> &GetSeasonPoster(int season);
    bool GetPartAndSeason(const int episodeId, int &season, int &part);
    int seriesID;
    int lastUpdated; // TODO: this should be a int64_t and a BIGINT in MySQL (year 2038 problem)
    string name;
    string banner;
    string fanart;
    string poster;
    string overview;
    string firstAired;
    string network;
    string imbdid;
    string genre;
    float rating = 0.0f;
    string status;
    void Dump(const int maxEntries = 0);
};

#endif //__TVSCRAPER_TMDBSERIES_H