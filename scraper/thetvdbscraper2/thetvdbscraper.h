#ifndef __TVSCRAPER_TVDBSCRAPER_H
#define __TVSCRAPER_TVDBSCRAPER_H
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <jansson.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tvdbmirrors.h"
#include "tvdbseries.h"
#include "tvdbepisode.h"
#include "thetvdbapi.h"

using namespace std;

// --- cTVDBScraper -------------------------------------------------------------

class cTVDBScraper {
private:
    string apiKey;
    string token;
    string apiUrl;
    string baseURL;
    string language;
    cTVDBApi tvdbapi;
    cTVDBMirrors *mirrors;
    xmlDoc *SetXMLDoc(string xml);
    int ParseXML(string xml);
    bool ParseToken(const string &s);
    int GetSeriesID(std::string &seriesData, string &seriesName);
public:
    cTVDBScraper(std::string language="de"); 
    virtual ~cTVDBScraper(void);
    bool Connect(void);
    // NEW API
    bool Login(const std::string &apiKey=std::string());
    bool UpdateToken(const std::string &token=std::string());
    bool ParseJSON(const std::string &jsonString, json_t **result);
    bool GetRequest(const std::string &url, std::string &jsonString, const std::string &lang="de");
    int ReadSeries(const std::string &seriesName);
    // END NEW API
    int GetServerTime(void);
    cTVDBSeries *ScrapInitial(std::string seriesName);
    bool GetUpdatedSeriesandEpisodes(set<int> *updatedSeries, set<int> *updatedEpisodes, int lastScrap);
    cTVDBSeries *GetSeries(int seriesID);
    cTVDBEpisode *GetEpisode(int episodeID, cTVDBApi &tvdbapi); 
};

#endif //__TVSCRAPER_TVDBSCRAPER_H
