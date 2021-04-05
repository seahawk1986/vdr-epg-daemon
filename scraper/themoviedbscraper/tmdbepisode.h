#ifndef __TVSCRAPER_TMDBEPISODE_H
#define __TVSCRAPER_TMDBEPISODE_H

#include <iostream>
#include <string>
#include <jansson.h>
#include "../../tools/stringhelpers.h"
#include "api_config.h"

class cTMDBEpisode {
    public:
        int id;
    private:
        std::shared_ptr<sApiData> _config;
        // void ParseXML(string xml);
    public:
    /*
    seriesID = 0;
    number = 0;
    season = 0;
    combinedEpisode = 0;
    combinedSeason = 0;
    name = "";
    firstAired = "";
    guestStars = "";
    overview = "";
    rating = 0.0;
    imageUrl = "";
    width = 400;
    height = 225;
    imgFlag = 0;
    seasonId = 0;
    lastUpdated = 0;
    */
        int seriesID = 0;
        int episode = 0;
        int season = 0;
        int number = 0;
        string firstAired;
        std::string name;
        std::string overview;
        std::string episode_first_started;
        std::string episode_guest_stars; // for the DB: "|Foo|Bar|Baz|"
        float rating = 0.0f;
        int last_updated;
        // int combinedSeason;   // TODO: unused, not in TMDB series data, TVDB only
        // int combinedEpisode;  // TODO: unused, not in TMDB series data, TVDB only
        string guestStars; // format: |foo|bar|baz|
        string imageUrl;
        int width = 400; // TODO: is this correct for TMDB series images?
        int height = 225;// TODO: is this correct for TMDB series images?
        int imgFlag = 0; // what does this do?
        int seasonId = 0;
        int lastUpdated = 0;
        // TODO: do we need this? Seems to be used when rescraping/updating data already in database...
        void ReadEpisode(void);
        // void ReadEpisodeFromXML(xmlDoc *myDoc, xmlNode *node, cTVDBMirrors *mirrors);
        cTMDBEpisode(void) = default; // TODO: where is this needed? Seems to be related to updating/rescraping episodes with partial data form DB
        cTMDBEpisode(int ID, const std::shared_ptr<sApiData> config); // get a single episode TODO: this needs a season for tmdb, too
        cTMDBEpisode(const cJsonNode &episodeData, const std::shared_ptr<sApiData> config);

        cTMDBEpisode(const cTMDBEpisode&) = delete;
        cTMDBEpisode(cTMDBEpisode&&) = default;

        void LoadEpisode(void);
        void ParseEpisode(const cJsonNode &episodeData);

        void Dump(void) {
            std::cout
                << "id: " << id << "\n"
                << "name: " << name << "\n"
                << "overview: " << overview << "\n"
                << "first_aired: " << firstAired << "\n"
                << "season: " << season << "\n"
                << "episode: " << episode << "\n"
                << "episode_first_started: " << episode_first_started << "\n"
                << "episode_guest_stars: " << episode_guest_stars << "\n"
                << "rating: " << rating << "\n"
                << "series_id: " << seriesID << "\n"
                << "last_updated: " << last_updated << "\n";
        };
};

#endif //__TVSCRAPER_TMDBEPISODE_H