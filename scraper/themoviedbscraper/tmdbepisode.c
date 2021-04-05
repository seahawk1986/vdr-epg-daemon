#include <sstream>
#include "tmdbepisode.h"

cTMDBEpisode::cTMDBEpisode(int ID, const std::shared_ptr<sApiData> config) : id(ID), _config(config)
{
    LoadEpisode();
}

cTMDBEpisode::cTMDBEpisode(const cJsonNode &episodeData, const std::shared_ptr<sApiData> config): _config(config)
{
    ParseEpisode(episodeData);
}

void cTMDBEpisode::LoadEpisode()
{
    std::ostringstream url;
    url << _config->baseUrl << "/tv/" << id
        << "/season/" << season
        << "/episode/" << episode
        << "?apikey=" << _config->apiKey
        << "&language=" << _config->language
        << "&append_to_response=images,credits";
}

void cTMDBEpisode::ParseEpisode(const cJsonNode &episodeData) {
    id = episodeData["id"].intValue();
    name = episodeData["name"].stringValue();
    overview = episodeData["overview"].stringValue();
    season = episodeData["season_number"].intValue();
    episode = episodeData["episode_number"].intValue();
    rating = episodeData["vote_average"].doubleValue();
    firstAired = episodeData["air_date"].stringValue();
    imageUrl = episodeData["still_path"].stringValue();
    guestStars = episodeData["guest_stars"].combine_jsonArrayStrings("|"); // |Guest A|Guest B|...|
}