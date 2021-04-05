#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <jansson.h>

#include "../../lib/curl.h"
#include "../../tools/stringhelpers.h"
#include "../../epgdconfig.h"
#include "thetmdbscraper.h"

using namespace std;

cTMDBScraper::cTMDBScraper(const std::shared_ptr<sApiData> config): _config(config)
{
    // apiKey = EpgdConfig.scrapMovieDbApiKey; // "abb01b5a277b9c2c60ec0302d83c5ee9";
    // baseURL = "api.themoviedb.org/3";
    posterSize = "w500";
    backdropSize = "w1280";
    actorthumbSize = "h632";
}

std::unique_ptr<cTMDBSeries> cTMDBScraper::ScrapInitial(const string &seriesName)
{
    // TODO: searching for a match should be done outside of cTMDBSeries 
    std::unique_ptr<cTMDBSeries> series = std::make_unique<cTMDBSeries>(seriesName, _config);
    if (series->seriesID)
    {
        return series;
    }
    return nullptr;
}

std::unique_ptr<cTMDBSeries> cTMDBScraper::GetSeries(int seriesID)
{
    return std::move(std::make_unique<cTMDBSeries>(seriesID, _config));
}

std::unique_ptr<cTMDBEpisode> cTMDBScraper::GetEpisode(int episodeID)
{ // TODO: this allows us only to check for changes
    return std::move(std::make_unique<cTMDBEpisode>(episodeID, _config));
}

bool cTMDBScraper::loadTvGenres(void)
{
    std::stringstream tvGenreUrl;
    tvGenreUrl << _config->baseUrl << "/3/genre/tv/list"
               << "?apikey=" << _config->apiKey << "&language=" << _config->language;

    std::string genresJson;
    if (curl.GetUrl(tvGenreUrl.str().c_str(), &genresJson))
    {
        auto data = cJsonNode(genresJson);
        if (data.isNull())
            return false;
        auto genre_array = data["genres"];
        // std::cout << json_dumps(genre_array.object(), JSON_INDENT(2)) << "\n";
        tv_genres.reserve(genre_array.arraySize());
        cJsonNode element;

        for (auto &element : genre_array.ArrayToVector())
        {
            int id = element.intByName("id");
            auto name = element.stringByName("name");
            tv_genres[id] = name;
        }
        return true;
    }
    return false;
}

// bool cTMDBScraper::Connect(void)
// {
//     return true;
//     // stringstream url;
//     // url << _config->baseUrl << "/configuration?api_key=" << _config->apiKey;
//     // string configJSON;
//     // if (curl.GetUrl(url.str().c_str(), &configJSON))
//     // {
//     //     auto data = cJsonNode(configJSON);
//     //     auto imgUrl = data["images"]["base_url"].stringValue();
//     //     if (imgUrl.empty())
//     //         return false;
//     //     this->imageUrl = imgUrl;
//     //     return true;
//     // }
//     // return false;
// }

int cTMDBScraper::GetServerTime(void)
{ // TODO: (Alexander) year 2038 problem, return a time_t or an int64_t
    return std::mktime(nullptr);
}

bool cTMDBScraper::GetUpdatedSeries(std::set<int> &updatedSeries, const char *start_date, const char *end_date)
{
    int page = 1;
    int total_pages;
    bool has_next_page = true;
    while (has_next_page)
    {
    }
    stringstream url;
    std::string result;
    url << _config->baseUrl << "/tv/changes"
        << "?api_key=" << _config->apiKey << "&page=" << page << "&start_date=" << curl.EscapeUrl(start_date) << "&end_date=" << curl.EscapeUrl(end_date);
    if (!curl.GetUrl(url.str().c_str(), &result))
        return false;
    cJsonNode updatedSeriesData = cJsonNode(result);
    if (updatedSeriesData.isNull())
        return false;
    for (const auto &series : updatedSeriesData["results"].ArrayToVector())
    {
        updatedSeries.insert(series["id"].intValue());
    }
    page = updatedSeriesData["page"].intValue();
    total_pages = updatedSeriesData["total_pages"].intValue();
    has_next_page = page < total_pages;
    return true;
}

bool cTMDBScraper::GetUpdatedSeasons(const std::set<int> &updatedSeries, std::set<std::pair<int, int>> &updatedSeasons, const char *start_date, const char *end_date)
{
    int page = 1;
    int total_pages = 1;
    bool has_next_page = true;
    for (const auto series_id : updatedSeries)
    {
        while (has_next_page)
        {
            stringstream url;
            std::string result;
            url << _config->baseUrl << "/tv/" << series_id << "/changes"
                << "?api_key=" << _config->apiKey << "&page=" << page << "&start_date=" << curl.EscapeUrl(start_date) << "&end_date=" << curl.EscapeUrl(end_date);
            if (!curl.GetUrl(url.str().c_str(), &result))
                return false;
            cJsonNode updatedSeasonData = cJsonNode(result);
            if (updatedSeasonData.isNull())
                return false;
            for (const auto &series : updatedSeasonData["changes"].ArrayToVector())
            {
                if (series["key"].stringValue() == "season")
                {
                    int season_id = series["items"]["value"]["season_id"].intValue();
                    int season_number = series["items"]["value"]["season_number"].intValue();
                    if (season_id != 0 && season_number != 0)
                    {
                        updatedSeasons.insert(std::make_pair(season_id, season_number));
                    }
                }
                page = updatedSeasonData["page"].intValue();
                total_pages = updatedSeasonData["total_pages"].intValue();
                has_next_page = page < total_pages;
            }
        }
    }
    return true;
}

bool cTMDBScraper::GetUpdatedEpisodes(const std::set<std::pair<int, int>> &updatedSeasons, std::set<std::pair<int, int>> &updatedEpisodes, const char *start_date, const char *end_date)
{
    int page = 1;
    int total_pages = 1;
    bool has_next_page = true;
    for (const auto season_data : updatedSeasons)
    {
        while (has_next_page)
        {
            stringstream url;
            std::string result;
            url << _config->baseUrl << "/tv/season/" << season_data.first << "/changes"
                << "?api_key=" << _config->apiKey << "&page=" << page
                << "&start_date=" << curl.EscapeUrl(start_date)
                << "&end_date=" << curl.EscapeUrl(end_date);
            if (!curl.GetUrl(url.str().c_str(), &result))
                return false;
            cJsonNode updatedSeasonData = cJsonNode(result);
            if (updatedSeasonData.isNull())
                return false;
            for (const auto &series : updatedSeasonData["changes"].ArrayToVector())
            {
                if (series["key"].stringValue() == "season")
                {
                    int season_id = series["items"]["value"]["episode_id"].intValue();
                    int season_number = series["items"]["value"]["episode_number"].intValue();
                    if (season_id != 0 && season_number != 0)
                        updatedEpisodes.insert(make_pair(season_id, season_number));
                }
            }
            page = updatedSeasonData["page"].intValue();
            total_pages = updatedSeasonData["total_pages"].intValue();
            has_next_page = page < total_pages;
        }
    }
    return true;
}

bool cTMDBScraper::GetUpdatedSeriesandEpisodes(set<int> &updatedSeries, set<std::pair<int, int>> &updatedEpisodes, int lastScrap)
{ // TODO: lastScrap should be a int64_t to be 2038 safe
    // TODO: move to TMDB API - https://developers.themoviedb.org/3/changes/get-tv-change-list
    //       increment in 14 days intervalls from the lastScrap to now and collect all seriesIDs; up to 100 items per page are returned

    std::time_t now_ts = std::time(nullptr);
    std::time_t begin, end;
    constexpr const std::time_t two_weeks = 14 * 24 * 3600;

    std::vector<std::pair<time_t, time_t>> vParseTimespans;
    begin = lastScrap;
    end = lastScrap + two_weeks;
    do
    {
        std::set<int> timeFrameUpdatedSeries;
        std::set<std::pair<int, int>> timeFrameUpdatedSeasons;
        std::set<std::pair<int, int>> timeFrameUpdatedEpisodes;
        char start_date[10], end_date[10];
        std::strftime(start_date, sizeof(start_date), "%m_%d_%Y", std::localtime(&begin));
        std::strftime(start_date, sizeof(start_date), "%m_%d_%Y", std::localtime(&end));
        GetUpdatedSeries(timeFrameUpdatedSeries, start_date, end_date);
        GetUpdatedSeasons(timeFrameUpdatedSeries, timeFrameUpdatedSeasons, start_date, end_date);
        GetUpdatedEpisodes(timeFrameUpdatedSeasons, timeFrameUpdatedEpisodes, start_date, end_date);
        // insert the ids for our timeframe in the final result
        updatedSeries.insert(timeFrameUpdatedSeries.begin(), timeFrameUpdatedSeries.end());
        updatedEpisodes.insert(timeFrameUpdatedEpisodes.begin(), timeFrameUpdatedEpisodes.end());
        begin += two_weeks;
        end += two_weeks;
    } while (end <= now_ts);

    return true;
}

std::unique_ptr<cTMDBSeries> cTMDBScraper::ReadSeries(const string &seriesName)
{
    // TODO: change to TMDB series search with series parsing
    int seriesID = 0;

    string seriesSearchResultJSON;
    char *escName = curl.EscapeUrl(seriesName.c_str());
    stringstream requestURL(_config->baseUrl);
    int page = 1;
    requestURL << "/search/tv?api_key=" << _config->apiKey << "&query=" << escName << "&language=" << _config->language.c_str();
    curl.Free(escName);

    bool read_next_page = false;

    while (read_next_page)
    {
        stringstream url (requestURL.str());
        url << "&page=" << page;
        if (curl.GetUrl(url.str().c_str(), &seriesSearchResultJSON)) {
            cJsonNode searchResult = cJsonNode(seriesSearchResultJSON);
            page = searchResult["page"].intValue();
            int total_pages = searchResult["total_pages"].intValue();
            read_next_page = (page < total_pages);
            std::vector<std::pair<int, int>> vMatches; // TODO: move out of loop
            vMatches.reserve(vMatches.size() + searchResult["total_results"].intValue());
            for (const auto &result: searchResult["results"].ArrayToVector()) {
                seriesID = result["id"].intValue();
                auto name = result["name"].stringValue();
                auto original_name = result["original_name"].stringValue();
                int tmp=60;

                // TODO: find the best match (e.g. calculate and store levenshtein-distance for each result) and select the best match in the result list
                //       instead of returning the first match.
                // auto distance = lvDistance(seriesName, name, 20, tmp);
                return std::make_unique<cTMDBSeries>(seriesID, _config);
            }

        }
        // seriesID = ParseJSON(seriesSearchResultJSON);
        return {};
    }

    return nullptr;
}