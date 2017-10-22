#ifndef __TVSCRAPER_TVDBEPISODE_H
#define __TVSCRAPER_TVDBEPISODE_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tvdbmirrors.h"
#include "thetvdbapi.h"

using namespace std; 

// --- cTVDBEpisode -------------------------------------------------------------
class cTVDBEpisode {
private:
    string apiKey;
    cTVDBMirrors *mirrors;
    cTVDBApi &tvdbapi;
    string language;
    void ParseXML(string xml);
public:
    cTVDBEpisode(cTVDBApi &tvdbapi)
      : tvdbapi(tvdbapi) {
        id = 0;
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
    }
    cTVDBEpisode(int ID, cTVDBApi &tvdbapi)
      : tvdbapi(tvdbapi) {
        id = ID;
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
    }
    int id;
    int seriesID;
    int number;
    int season;
    int combinedSeason;
    int combinedEpisode;
    string name;
    string firstAired;
    string guestStars;
    string overview;
    float rating;
    string imageUrl;
    int width;
    int height;
    int imgFlag;
    int seasonId;
    int lastUpdated;
    void ReadEpisode(void);
    void ReadEpisodeFromXML(xmlDoc *myDoc, xmlNode *node, cTVDBMirrors *mirrors);
    void Dump();
};
#endif //cTVDBEpisode
