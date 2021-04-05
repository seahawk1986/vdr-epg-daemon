#include "tmdbmedia.h"

cTMDBMedia::cTMDBMedia(const cJsonNode &data, const std::shared_ptr<sApiData> &config): _config(config) {
    url = data.stringByName("file_path");
    height = data.intByName("height");
    width = data.intByName("width");
    language = data.stringByName("iso_639_1");
    rating = data.doubleByName("vote_average");
}

cTMDBMedia::cTMDBMedia(const std::shared_ptr<sApiData> &config): _config(config) {}

cTMDBFanart::cTMDBFanart(const cJsonNode &data, const std::shared_ptr<sApiData> config): cTMDBMedia(data, config) {
    url = config->mediaBaseUrl + _config->backdrop_sizes.back() + url;
}

cTMDBPoster::cTMDBPoster(const cJsonNode &data, const std::shared_ptr<sApiData> config): cTMDBMedia(data, config) {
    url = config->mediaBaseUrl + _config->poster_sizes.back() + url;
}

cTMDBBanner::cTMDBBanner(const cJsonNode &data, const std::shared_ptr<sApiData> config): cTMDBMedia(data, config) {
    url = config->mediaBaseUrl + _config->logo_sizes.back() + url;
}

cTMDBSeasonPoster::cTMDBSeasonPoster(int seasonNumber, const std::string &url_path, const std::shared_ptr<sApiData> &config): cTMDBMedia(config), season(seasonNumber) {
    url = config->mediaBaseUrl + config->poster_sizes.back() + url_path;
}


cTMDBSeriesMedia::cTMDBSeriesMedia(const std::shared_ptr<sApiData> config): _config(config) {}

void cTMDBSeriesMedia::ParseJson(const std::string &data, vector<std::unique_ptr<cTMDBBanner>> &banners, vector<std::unique_ptr<cTMDBFanart>> &fanarts, vector<std::unique_ptr<cTMDBPoster>> &posters, vector<std::unique_ptr<cTMDBSeasonPoster>> &seasonPosters) {
    ParseJson(cJsonNode(data), banners, fanarts, posters, seasonPosters);
}

void cTMDBSeriesMedia::ParseJson(const cJsonNode &data, vector<std::unique_ptr<cTMDBBanner>> &banners, vector<std::unique_ptr<cTMDBFanart>> &fanarts, vector<std::unique_ptr<cTMDBPoster>> &posters, vector<std::unique_ptr<cTMDBSeasonPoster>> &seasonPosters) {
    // TODO: implement
    for (const auto & backdrop: data["backdrops"].ArrayToVector()) {

         // TODO: sort by language property
        fanarts.emplace_back(std::make_unique<cTMDBFanart>(backdrop, _config));
    }
    for (const auto & backdrop: data["posters"].ArrayToVector()) {

         // TODO: sort by language property
        posters.emplace_back(std::make_unique<cTMDBPoster>(backdrop, _config));
    }

    // TODO: there is no api to retrieve banners/show logos :(
    // for (const auto & banner: data["banners"].ArrayToVector()) {
    //     // TODO: sort by language property
    //     banners.emplace_back(std::make_unique<cTMDBBanner>(banner, _config));
    // }

}