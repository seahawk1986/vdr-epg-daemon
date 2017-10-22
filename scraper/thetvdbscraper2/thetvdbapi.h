#ifndef __TVSCRAPER_TVDBAPI_H
#define __TVSCRAPER_TVDBAPI_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <jansson.h>
#include "../../lib/curl.h"
#include "../../levenshtein.h"

using namespace std;

// --- cTVDBApi -------------------------------------------------------------
//
// This class encapsulates the new TheTVDB API (https://api.thetvdb.com/swagger)
// and the libhorchi curl requests.
// You need to initialize this class with an apikey and the default language.
// Login() will request a token which is used for the following GetRequest() Calls.
// The token can be used up to 24h and should be updated using RefreshToken(),
// so it will not expire. GetRequest() stores the time of the last successfull
// authenticated request and updates the token if needed.
//
// example:
// cTVDBApi tvdbapi("mySecretAPIKey", "de", "en");
// tvdbapi.Login("mySecretAPIKey");
// tvdbapi.GET(url, requestedData);

class cTVDBApi {
private:
    string defaultLanguage;
    string apikey;
    string language;
    string fallback_language;

    string token;

    string baseURL;
    string bannerURL;
    string posterURL;

    time_t last_request;
    // old api
    vector<string> xmlmirrors;
    vector<string> bannermirrors;
    /*
    bool ReadEntry(xmlDoc *doc, xmlNode *node);
    bool CreateMirror(string path, int typemask);
    */
    // end old api
    bool ParseToken(const string &s);
    int GetSeriesID(std::string &seriesData, string &seriesName);
public:
    cTVDBApi();
    virtual ~cTVDBApi(void);
    // new api
    const string apiUrl = "https://api.thetvdb.com";
    bool Login(std::string apiKey, std::string language="de");
    bool UpdateToken(const std::string &token=std::string());
    bool ParseJSON(const std::string &jsonString, json_t **result);
    bool GetRequest(const std::string &url, std::string &jsonString, const std::string &lang="de");
    bool GetJSONRequest(const std::string &url, json_t **jsonData, const std::string &lang);
    // new api
    int ReadSeries(const std::string &seriesName);
    /*
    string GetMirrorXML(void);
    string GetMirrorBanner(void);
    */
};


#endif //__TVSCRAPER_TVDBAPI_H
