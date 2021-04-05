#ifndef __TVSCRAPER_TMDBSCRAPER_H
#define __TVSCRAPER_TMDBSCRAPER_H
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
// #include <libxml/parser.h>
// #include <libxml/tree.h>
#include "../../levenshtein.h"
#include "tmdbseries.h"
#include "api_config.h"

using namespace std;

// --- cTMDBScraper -------------------------------------------------------------

class cTMDBScraper {
private:
    const std::shared_ptr<sApiData> _config;
    std::unique_ptr<cTMDBSeries> ReadSeries(const string &seriesName);
    bool loadTvGenres(void);
    bool GetUpdatedSeries(std::set<int> &updatedSeries, const char* start_date, const char* end_date);
    bool GetUpdatedSeasons(const std::set<int> &updatedSeries, std::set<std::pair<int, int>> &updatedSeasons, const char* start_date, const char* end_date);
    bool GetUpdatedEpisodes(const std::set<std::pair<int, int>> &updatedSeasons, std::set<std::pair<int, int>> &updatedEpisodes, const char* start_date, const char* end_date);
public:
    string posterSize;
    string backdropSize;
    string actorthumbSize;
    std::vector<std::string> backdrop_sizes;
    std::vector<std::string> logo_sizes;
    std::vector<std::string> poster_sizes;
    std::vector<std::string> profile_sizes;
    std::vector<std::string> still_sizes;
    std::unordered_map<int, std::string> tv_genres;
    cTMDBScraper(const std::shared_ptr<sApiData> config);
    virtual ~cTMDBScraper(void) = default;
    // bool Connect(void);
    int GetServerTime(void);
    std::unique_ptr<cTMDBSeries> ScrapInitial(const string &seriesName);
    bool GetUpdatedSeriesandEpisodes(set<int> &updatedSeries, set<std::pair<int, int>> &updatedEpisodes, int lastScrap);
    std::unique_ptr<cTMDBSeries> GetSeries(int seriesID);
    std::unique_ptr<cTMDBEpisode> GetEpisode(int episodeID); 
};


#endif //__TVSCRAPER_TMDBSCRAPER_H
