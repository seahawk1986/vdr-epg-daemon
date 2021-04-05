#ifndef __TVSCRAPER_TMDBMEDIA_H
#define __TVSCRAPER_TMDBMEDIA_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
// #include <libxml/parser.h>
// #include <libxml/tree.h>
#include "api_config.h"

#include "../../tools/stringhelpers.h"


using namespace std; 

// --- cTVDBMedia -------------------------------------------------------------
class cTMDBMedia {
public:
    string url;
    string language;
    int width = 0, height = 0;
    double rating = 0.0f;
    cTMDBMedia(void) {};
    cTMDBMedia(const std::shared_ptr<sApiData> &config);
    cTMDBMedia(const cJsonNode &data, const std::shared_ptr<sApiData> &config);
    const std::shared_ptr<sApiData> _config;
    virtual void Dump() {
        cout << "cTMDBMedia Base Class Dump.\n";
    };
};

// --- cTVDBFanart -------------------------------------------------------------
class cTMDBFanart : public cTMDBMedia {
public:
    cTMDBFanart(void) {
        thumbUrl = "";
    };
    cTMDBFanart(const cJsonNode &data, const std::shared_ptr<sApiData> config);
    string thumbUrl;
    void Dump() {
        cout << "Url: " << url << endl;
        cout << "Url Thumbnail: " << thumbUrl << endl;
        cout << "Language: " << language << endl;
        cout << "Size: " << width << " x " << height << endl;
        cout << "Rating: " << rating << endl;
    };
};

// --- cTVDBPoster -------------------------------------------------------------
class cTMDBPoster : public cTMDBMedia {
public:
    cTMDBPoster(void) {
    };
    cTMDBPoster(const cJsonNode &data, const std::shared_ptr<sApiData> config);
    void Dump() {
        cout << "Url: " << url << endl;
        cout << "Language: " << language << endl;
        cout << "Size: " << width << " x " << height << endl;
        cout << "Rating: " << rating << endl;
    };
};

// --- cTMDBSeasonPoster -------------------------------------------------------------
class cTMDBSeasonPoster : public cTMDBMedia {
public:
    cTMDBSeasonPoster(void) {
        season = 0;
    };
    cTMDBSeasonPoster(int seasonNumber, const std::string &url_path, const std::shared_ptr<sApiData> &config);
    cTMDBSeasonPoster(const cJsonNode &data, const std::shared_ptr<sApiData> config);
    cTMDBSeasonPoster(const cTMDBSeasonPoster&) = delete;
    cTMDBSeasonPoster(cTMDBSeasonPoster&&) = default;

    int season = 0;
    void Dump() {
        cout << "Url: " << url << endl;
        cout << "Season: " << season << endl;
        cout << "Language: " << language << endl;
        cout << "Size: " << width << " x " << height << endl;
        cout << "Rating: " << rating << endl;
    };
};

// --- cTMDBBanner -------------------------------------------------------------
class cTMDBBanner : public cTMDBMedia {
public:
    cTMDBBanner(void) = default;
    cTMDBBanner(const cJsonNode &data, const std::shared_ptr<sApiData> config);
    int season;
    void Dump() {
        cout << "Url: " << url << endl;
        cout << "Language: " << language << endl;
        cout << "Size: " << width << " x " << height << endl;
        cout << "Rating: " << rating << endl;
        cout << "Season: " << season << endl;
    };
};

// --- cTMDBSeriesMedia --------------------------------------------------------

class cTMDBSeriesMedia { // TODO: this probably best corresponds to the image objects in the json array
private:
    std::shared_ptr<sApiData> _config;
    std::vector<std::unique_ptr<cTMDBBanner>> banners; // TODO: (alexander) how to access those? We could try https://fanart.tv/
    std::vector<std::unique_ptr<cTMDBMedia>>fanarts;
    std::vector<std::unique_ptr<cTMDBPoster>> posters;
    std::vector<std::unique_ptr<cTMDBSeasonPoster>> seasonPosters;
    void ReadEntry(cJsonNode);
    // TODO: delete unneeded declarations
    // TODO: replace those methods
    // void ReadEntry(xmlDoc *doc, xmlNode *node);
    // void ReadFanart(xmlDoc *doc, xmlNode *node);
    // void ReadPoster(xmlDoc *doc, xmlNode *node);
    // void ReadBanner(xmlDoc *doc, xmlNode *node);
    // void ReadSeasonPoster(xmlDoc *doc, xmlNode *node);
public:
    cTMDBSeriesMedia(const std::shared_ptr<sApiData> config);
    virtual ~cTMDBSeriesMedia(void) = default;
    // void ParseXML(string xml, vector<cTMDBBanner*> *banners, vector<cTMDBFanart*> *fanarts, vector<cTMDBPoster*> *posters, vector<cTMDBSeasonPoster*> *seasonPosters);
    void ParseJson(const std::string &data, vector<std::unique_ptr<cTMDBBanner>> &banners, vector<std::unique_ptr<cTMDBFanart>> &fanarts, vector<std::unique_ptr<cTMDBPoster>> &posters, vector<std::unique_ptr<cTMDBSeasonPoster>> &seasonPosters);
    void ParseJson(const cJsonNode &data, vector<std::unique_ptr<cTMDBBanner>> &banners, vector<std::unique_ptr<cTMDBFanart>> &fanarts, vector<std::unique_ptr<cTMDBPoster>> &posters, vector<std::unique_ptr<cTMDBSeasonPoster>> &seasonPosters);

};

#endif //__TVSCRAPER_TMDBMEDIA_H
