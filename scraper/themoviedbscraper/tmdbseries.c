#include <algorithm>
#include <ctime>
#include <memory>
#include <sstream>

#include "../../lib/curl.h"
#include "../../tools/stringhelpers.h"

#include "tmdbseries.h"
#include "tmdbepisode.h"
#include "tmdbactor.h"

using namespace std;

cTMDBSeries::cTMDBSeries (const int ID, const std::shared_ptr<sApiData> config): _config(config), seriesID(ID) {
    this->language_order[_config->language] = 0;

}


// TODO: do we need this? Searching for a series should be the job of the thetmdbscraper...
cTMDBSeries::cTMDBSeries(const string &seriesName, std::shared_ptr<sApiData> config): _config(config) {
    this->name = seriesName;
    this->language_order[_config->language] = 0;
}

cTMDBSeries::cTMDBSeries(const cJsonNode &data, const std::shared_ptr<sApiData> config): _config(config) {
    ReadSeriesData(data);
    // seriesID = data.intByName("id");
    // name = data.stringByName("name");
    // fanart = data.stringByName("backdrop_path");
    // firstAired = data.stringByName("first_air_date");
    // network = data["networks"].combine_jsonArrayStrings(",", false);


}

bool cTMDBSeries::ReadSeries(void) {
    stringstream url;
    url << _config->baseUrl << "/3/tv/" << seriesID
        << "?api_key=" << _config->apiKey
        << "&language=" << _config->language
        << "&append_to_response=external_ids,images,credits";
    url << "&include_image_language=" << "en,null";
    string seriesJson;
    std::cerr << "Series URL: " << url.str() << "\n";
    bool read_next_page = true;
    while (read_next_page) {
        if (curl.GetUrl(url.str().c_str(), &seriesJson)) {
            cJsonNode jsonData(seriesJson.c_str());

            if (!jsonData.isObject()) {
                return false;
            }
            ReadSeriesData(jsonData);
        }
        return true;
    }
    return false;
}


void cTMDBSeries::ReadMedia(void) {
    stringstream url;
    url << _config->baseUrl << "/3/tv/" << seriesID << "/images" << "?api_key=" << _config->apiKey << "&language=" << _config->language;
    url << "&include_image_language=" << "en,null";
    string imagesJson;
    std::cerr << "Media Url: " << url.str() << "\n";
    if (curl.GetUrl(url.str().c_str(), &imagesJson)) {
        ReadMedia(cJsonNode(imagesJson));
    } else {
        std::cout << "get request for media failed: " << url.str() << "\n";
    }
}

void cTMDBSeries::ReadMedia(const cJsonNode &data) {
    cTMDBSeriesMedia med(_config);
    med.ParseJson(data, banners, fanarts, posters, seasonPosters);  
    SortPosters();
    SortFanart();  
}


void cTMDBSeries::ReadActors(void) {
    stringstream url;
    url << _config->baseUrl << "/3/tv/" << seriesID << "/credits" << "?api_key=" << _config->apiKey << "&language=" << _config->language;
    std::cerr << "Actors URL: " << url.str() << "\n";
    string actorsJson;
    if (curl.GetUrl(url.str().c_str(), &actorsJson)) {
        cTMDBActors act(_config);
        act.ParseJson(actorsJson, actors);
    }
}

void cTMDBSeries::ReadSeriesData(const cJsonNode &data) {
    seriesID = data.intByName("id");

    std::stringstream ssGenres;
    ssGenres << "|";
    for (const auto &genreElement: data["genres"].ArrayToVector()) {
        ssGenres << genreElement.stringByName("name");
        ssGenres << "|";

    }
    genre = ssGenres.str();

    cTMDBActors actorsParser(_config);
    actorsParser.ParseJson(data["credits"]["cast"], actors);

    network = data["networks"].combine_jsonArrayStrings(",", false);
    firstAired = data.stringByName("first_aired");
    name = data.stringByName("name");
    overview = data.stringByName("overview");
    rating = data.doubleByName("rating");
    status = data.stringByName("status");
    imbdid = data["external_ids"].stringByName("imdb_id");

    if (!data["logo_path"].isNull()) {
        banner = _config->mediaBaseUrl + _config->logo_sizes.back() + data.stringByName("logo_path");
        auto pBanner = std::make_unique<cTMDBBanner>();
        pBanner->language = _config->language;
        pBanner->url = banner;
        banners.push_back(std::move(pBanner));
    }
    if (!data["backdrop_path"].isNull()) fanart = _config->mediaBaseUrl + _config->backdrop_sizes.back() + data.stringByName("backdrop_path");
    if (!data["poster_path"].isNull()) poster = _config->mediaBaseUrl + _config->poster_sizes.back() + data.stringByName("poster_path");
    ReadMedia(data["images"]);
    // use current timestamp, since there is no easy way to get the last update time from  the tmdb
    lastUpdated = static_cast<int>(std::time(0)); // TODO: cast to int64_t to be year 2038 safe

    for (const auto& season: data["seasons"].ArrayToVector()) {
        ParseSeason(season);
    }

}

void cTMDBSeries::ParseSeason(const cJsonNode &jSeason) {
    // TODO: (alexander) are those needed?
    // int seasonID = jSeason.intByName("id");
    // int episodeCount = jSeason.intByName("episode_count");
    int seasonNumber = jSeason.intByName("season_number");
    auto seasonPosterPath = jSeason.stringByName("poster_path");
    if (!seasonPosterPath.empty()) {
        auto pSeasonPoster = std::make_unique<cTMDBSeasonPoster>(seasonNumber, seasonPosterPath, _config); // TODO: a custom constructor for this case would be nice
        seasonPosters.push_back(std::move(pSeasonPoster));
    }
    cJsonNode jEpisodeData = LoadSeason(seasonNumber);
    if (jEpisodeData.isNull()) {
        tell(0, "Loading Episodes for series %d season %d failed.", seriesID, seasonNumber);
        return;
    }
    for (const auto &episode: jEpisodeData["episodes"].ArrayToVector()) {
        ParseEpisode(episode);
    }
}

cJsonNode cTMDBSeries::LoadSeason(const int seasonNumber) {
    stringstream url;
    url << _config->baseUrl
        << "/3/tv/" << seriesID
        << "/season/" << seasonNumber
        << "?api_key=" << _config->apiKey
        << "&language=" << _config->language
        << "&append_to_response=credits,images";
    string episodeJson;
    if (curl.GetUrl(url.str().c_str(), &episodeJson)) return cJsonNode(episodeJson);
    return cJsonNode();
}

void cTMDBSeries::ParseEpisode(const cJsonNode &episodeData) {
    episodes.emplace_back(std::make_unique<cTMDBEpisode>(episodeData, _config));
}

const vector<std::unique_ptr<cTMDBEpisode>> &cTMDBSeries::GetEpisode(void) {
    return episodes;
}

const std::vector<std::unique_ptr<cTMDBActor>> &cTMDBSeries::GetActor(void) {
    return actors;
}

const vector<std::unique_ptr<cTMDBFanart>> &cTMDBSeries::GetFanart(void) {
    return fanarts;
}

const vector<std::unique_ptr<cTMDBPoster>> &cTMDBSeries::GetPoster(void) {
    return posters;
}

template <class T>
void cTMDBSeries::SortMedia(std::vector<std::unique_ptr<T>> &vMedia) {
    static_assert(std::is_base_of<cTMDBMedia, T>::value, "T must derive from cTMDBMedia");
    std::sort(vMedia.begin(), vMedia.end(), [this] (const std::unique_ptr<T> & a, const std::unique_ptr<T> &b) {
        auto compare_a = language_order[a->language];
        auto compare_b = language_order[b->language];
        if (compare_a != compare_b) {
            return compare_a < compare_b;
        } else {
            return a->rating < b->rating;
        }
    });
}

void cTMDBSeries::SortPosters(void) {
    SortMedia(posters);
}

void cTMDBSeries::SortFanart(void) {
    SortMedia(fanarts);
}

const std::vector<std::unique_ptr<cTMDBSeasonPoster>> &cTMDBSeries::GetSeasonPoster(void) {
    return seasonPosters;
}

const std::unique_ptr<cTMDBSeasonPoster> &cTMDBSeries::GetSeasonPoster(int season) { // TODO: this has O(n) - should seasonPosters be a unordered_map with the season as index?
    static unique_ptr<cTMDBSeasonPoster> not_found;
    for (const auto &poster: seasonPosters) {
        if (poster->season == season) return poster;
    }
    return not_found;
}

const std::vector<std::unique_ptr<cTMDBBanner>> &cTMDBSeries::GetBanner(void) {
    return banners;
}

bool cTMDBSeries::GetPartAndSeason(const int episodeId, int &season, int &part) { 
    for (auto const &episode: episodes) {
        if (episode->id == episodeId) {
            season = episode->season;
            part = episode->number;
            return true;
        }
    }
    return false;
}

void cTMDBSeries::Dump(const int maxEntries) {
    cout << "--------------------------- Series Info ----------------------------------" << endl;
    cout << "series " << name << ", ID: " << seriesID <<endl;
    cout << "Overview: " << overview << endl;
    cout << "Banner: " << banner << endl;
    cout << "Poster: " << poster << endl;
    cout << "Fanart: " << fanart << endl;
    cout << "imdb: " << imbdid << endl;
    cout << "FirstAired: " << firstAired << endl;
    cout << "Network: " << network << endl;
    cout << "Status: " << status << endl;
    cout << "lastUpdated: " << lastUpdated << endl;
    cout << "Genre: " << genre << endl;
    cout << "Rating: " << rating << endl;


    int size = episodes.size();
    cout << "--------------------------- " << size << " episodes ----------------------------------" << endl;
    for (int i=0; i<size; i++) {
        if (maxEntries && (i == maxEntries)) break;
        episodes[i]->Dump();
    }

    size = actors.size();
    cout << "--------------------------- " << size << " actors ----------------------------------" << endl;
    for (int i=0; i<size; i++) {
        if (maxEntries && (i == maxEntries)) break;
        actors[i]->Dump();
    }

    int numFanart = fanarts.size();
    cout << "--------------------------- " << numFanart << " fanarts ----------------------------------" << endl;
    for (int i=0; i<numFanart; i++) {
        if (maxEntries && (i == maxEntries)) break;
        fanarts[i].get()->Dump();
    }
    int numPoster = posters.size();
    cout << "--------------------------- " << numPoster << " posters ----------------------------------" << endl;
    for (int i=0; i<numPoster; i++) {
        if (maxEntries && (i == maxEntries)) break;
        posters[i].get()->Dump();
    }
    int numSeasonPoster = seasonPosters.size();
    cout << "--------------------------- " << numSeasonPoster << " season posters ---------------------------" << endl;
    for (int i=0; i<numSeasonPoster; i++) {
        if (maxEntries && (i == maxEntries)) break;
        seasonPosters[i]->Dump();
    }
    int numBanner = banners.size();
    cout << "--------------------------- " << numBanner << " banners ----------------------------------" << endl;
    for (int i=0; i<numBanner; i++) {
        if (maxEntries && (i == maxEntries)) break;
        banners[i].get()->Dump();
    }    
}
