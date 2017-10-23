#ifndef __TVSCRAPER_TVDBSERIES_H
#define __TVSCRAPER_TVDBSERIES_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <jansson.h>
#include "tvdbmirrors.h"
#include "tvdbactor.h"
#include "tvdbmedia.h"
#include "tvdbepisode.h"
#include "thetvdbapi.h"

using namespace std;

// --- cTVDBSeries -------------------------------------------------------------

class cTVDBSeries {
private:
    cTVDBApi &tvdbapi;
    string language;
    vector<cTVDBEpisode*> episodes;
    vector<cTVDBActor*> actors;
    vector<cTVDBFanart*> fanarts;
    vector<cTVDBPoster*> posters;
    vector<cTVDBSeasonPoster*> seasonPosters;
    vector<cTVDBBanner*> banners;
    int currentEpisode;
    int currentActor;
    int currentFanart;
    int currentPoster;
    int currentSeasonPoster;
    int currentBanner;
    bool ParseSeries(json_t **data);
    void ParseEpisodes(json_t **data);
    void ParseXML(string xml);
    void ReadSeriesData(xmlDoc *doc, xmlNode *node);
    void SetGenre(json_t **data, string &result);
public:
    cTVDBSeries(int ID, cTVDBApi &tvdbapi)
      : tvdbapi(tvdbapi) {
            seriesID = ID;
            name = "";
            banner = "";
            fanart = "";
            poster = "";
            overview = "";
            firstAired = "";
            network = "";
            imbdid = "";
            genre = "";
            rating = 0.0;
            status = "";
            currentEpisode = 0;
            currentActor = 0;
            currentFanart = 0;
            currentPoster = 0;
            currentSeasonPoster = 0;
            currentBanner = 0;
    }
    virtual ~cTVDBSeries(void);
    bool ReadSeries(void);
    bool GetSeries(void);
    bool GetEpisodes(void);
    void ReadMedia(void);
    void ReadActors(void);
    void SetClassValue(const string &fieldName, json_t **data, string &result);
    void SetClassValue(const string &fieldName, json_t **data, float &result);
    void SetInt2IntClassValue(const string &fieldName, json_t **data, int &result);
    cTVDBEpisode *GetEpisode(void);
    cTVDBActor *GetActor(void);
    cTVDBFanart *GetFanart(void);
    cTVDBPoster *GetPoster(void);
    cTVDBSeasonPoster *GetSeasonPoster(void);
    cTVDBSeasonPoster *GetSeasonPoster(int season);
    cTVDBBanner *GetBanner(void);
    bool GetPartAndSeason(int episodeId, int &season, int &part);
    int seriesID;
    int lastUpdated;
    string name;
    string banner;
    string fanart;
    string poster;
    string overview;
    string firstAired;
    string network;
    string imbdid;
    string genre;
    float rating;
    string status;
    void Dump(int maxEntries = 0);
};


#endif //__TVSCRAPER_TVDBSERIES_H
