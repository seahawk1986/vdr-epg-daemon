#ifndef __TVSCRAPER_TMDBACTORS_H
#define __TVSCRAPER_TMDBACTORS_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
// #include <libxml/parser.h>
// #include <libxml/tree.h>
#include "../../tools/stringhelpers.h"
#include "api_config.h"

using string = std::string;

class cTMDBActor;
using vActors = std::vector<std::unique_ptr<cTMDBActor>>;
//using namespace std; 

// TODO: we could use a joint Table for both movie and tv actors - same data, same ids, mostly similar fields

// --- cTMDBActor -------------------------------------------------------------
class cTMDBActor {
public:
    cTMDBActor(void) = default;
    cTMDBActor(const std::shared_ptr<sApiData> config): _config(config) {};
    cTMDBActor(const cJsonNode &data, const std::shared_ptr<sApiData> _config);
    string mediaBaseURL;
    string thumbUrl;
    int thumbUrlWidth = 340;  // TODO: why do we need this?
    int thumbUrlHeight = 450; // TODO: why do we need this?
    string name;
    string role;
    int id = 0;
    int sortOrder = 0;
    void Dump() {
        cout << "Actor name: " << name << ", ID: " << id << "\n";
        cout << "Actor role: " << role << "\n";
        cout << "Actor thumb: " << thumbUrl << "\n";
        cout << "Actor SortOrder: " << sortOrder << endl;
    }
    private:
    const std::shared_ptr<sApiData> _config;
};

// --- cTMDBActors --------------------------------------------------------

class cTMDBActors {
private:
    const std::shared_ptr<sApiData> _config;

public:
    cTMDBActors(const std::shared_ptr<sApiData> config): _config(config) {};
    virtual ~cTMDBActors(void) = default;
    void ParseJson(const string &jsonData, vActors &actors);
    void ParseJson(const cJsonNode &jsonData, vActors &actors);
};

#endif //__TVSCRAPER_TMDBACTORS_H