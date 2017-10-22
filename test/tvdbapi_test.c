#include <iostream>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../scraper/thetvdbscraper2/thetvdbscraper.h"
#include "../scraper/thetvdbscraper2/thetvdbapi.h"
#include "../levenshtein.h"
#include "../lib/curl.h"
#include "../tools/stringhelpers.h"
#include "tvdbapi_test.h"

const static string api_url = "https://api.thetvdb.com";
string searchArg;

string jsonSeriesExample = R"foo(
{
  "data": [
    {
      "aliases": [],
      "banner": "graphical/71020-g.jpg",
      "firstAired": "1961-01-07",
      "id": 71020,
      "network": "ITV",
      "overview": "John Steed ist Geheimagent im Auftrag Ihrer Majestät und mit besonders schwierigen Aufgaben der Verbrechensbekämpfung betraut. Ein simpler Mord ist das Geringste, was ihn beschäftigt, eher geht es um größenwahnsinnige Wissenschaftler, exzentrische Verschwörer, wirre Fanatiker und generell das Streben nach Weltherrschaft. Zur Seite steht ihm Emma Peel, eine Karate-Lady, die ihre Kunst im Kampf gegen Ganoven regelmäßig einsetzt. Steed selbst benutzt seinen Regenschirm als Waffe, in dem ein Degen versteckt ist, und macht selbst in diesen Fällen eine elegante Figur. Er fährt einen Bentley, trägt stets einen maßgeschneiderten dunklen Anzug und eine meist mit Eisen gefütterte Melone und legt Wert darauf, dass sein Kaffee nur gegen den Uhrzeigersinn umgerührt wird. Emma Peel fährt einen Lotus Elan, trägt außergewöhnliche, eng anliegende, figurbetonende Kleider und kniehohe Lederstiefel. Auch ihre Arbeitsweise ist eher unkonventionell.",
      "seriesName": "Mit Schirm, Charme und Melone",
      "status": "Ended"
    },
    {
      "aliases": [
        "Mit Schirm, Charme und Melone 1976"
      ],
      "banner": "graphical/4010-g.jpg",
      "firstAired": "1976-10-01",
      "id": 76360,
      "network": "ITV1",
      "overview": "John Steed ist wieder da. Zusammen mit seinen beiden neuen Partnern Mike Gambit und Purdey macht er sich erneut auf die Jagd nach Doppelagenten und \"Diabolical Masterminds\". ",
      "seriesName": "Mit Schirm, Charme und Melone (1976)",
      "status": "Ended"
    }
  ]
}
)foo";
/*
bool parseJSON(const string &data, json_t **result)
{
    json_error_t error;
    *result = json_loads(data.c_str(), 0, &error);
    if (!result) {
   fprintf(stderr, "error on line %d: %s", error.line, error.text);
        return false;
    }
    return true;
}

bool getSeries(const std::string &token, int seriesID, std::string &jsonResult, const std::string &language)
{
   const std::string url(api_url + "/series/" + std::to_string(seriesID));

   if (getRequest(token, url, jsonResult, language)) {
      //parseSeriesJSON(seriesJSON);
      cout << "Series data:\n" << jsonResult << '\n';
      return true;
   } else {
      return false;
   }
}

bool getEpisodes(const string &token, int seriesID, string &jsonResult, const string &language)
{
   // get Episodes for a given seriesID
   stringstream url;

   string jsonPage;
   int nextPage = 1;
   json_auto_t *links, *pageData, *next_field;
   json_auto_t *root, *data = json_array();
   size_t dataSize;
   while (nextPage > 0) {
      cout << "fetching page " << nextPage << '\n';
      url << api_url << "/series/" << seriesID << "/episodes?page=" << nextPage;
      if (getRequest(token, url.str(), jsonPage, language)) {
         //cout << "Series data:\n" << jsonPage << '\n';
         if (parseJSON(jsonPage, &root)) {
            int index;
            json_auto_t *value;
            pageData = json_object_get(root, "data"); 
            json_array_foreach(pageData, index, value) {
               json_array_append(data, value);
            }
            links = json_object_get(root, "links"); 
            if (!links) {
               cout << "no links element found" << '\n';
               break;
            }
            next_field = json_object_get(links, "next");
            if (!next_field) {
               cout << "no next element found" << '\n';
               break;
            }
            nextPage = json_integer_value(next_field);
            cout << "next Page is " << nextPage << '\n';
         } else {
            nextPage = 0;
            break;
         }
      } else {
         break;
      }
      url.str(std::string());
   }
   if (data) {
      cout << "got " << json_array_size(data) << " episodes" << '\n';
      jsonResult = json_dumps(data, 0);
      return true;
   } else {
      return false;
   }
}

bool getActors(const string &token, int seriesID, string &jsonResult, const string &language)
{
   // get actors for a seriesID
   // TODO Parse array objects into Actor Objects
   stringstream url;
   url << api_url << "/series/" << seriesID << "actors";

   if (getRequest(token, url.str(), jsonResult, language)) {
      cout << "Series data:\n" << jsonResult << '\n';
      return true;
   } else {
      return false;
   }
}
   
bool getImages(const string &token, int seriesID, string &jsonResult, const string &language)
{
   // returns data for the number of available images of each type
   stringstream url;
   url << api_url << "/series/" << seriesID << "images";

   if (getRequest(token, url.str(), jsonResult, language)) {
      cout << "Series data:\n" << jsonResult << '\n';
      return true;
   } else {
      return false;
   }
}
*/

int main(int argc, char *argv[])
{
   string token = "";
   string apikey = "E9DBB94CA50832ED";
   //   cTVDBApi tvdbapi;
   cTVDBScraper scraper;

   if (!scraper.Connect()) {
       std:cout << "Login failed" << '\n';
   return 1;
   }

   /*
   if (!tvdbapi.UpdateToken()) {
       std::cout << "refreshing Token failed" << '\n';
   return 1;
   }
   */

   if (argc > 1) {
      searchArg = argv[1];
      std::cout << "argument was: " << searchArg << std::endl;
   } else {
      searchArg = "Mit Schirm, Charme und Melone";
   }

   int seriesID;
   seriesID = scraper.ReadSeries(searchArg);
   if (seriesID == 0) {
      cout << "search failed" << '\n';
      return 1;
   }
   cout << "SeriesID for " << searchArg << " is " << seriesID << '\n';
   /*
   if (getSeries(token, seriesID, jsonData)) {
      string episodeArray;
      getEpisodes(token, seriesID, episodeArray);
      cout << "episoden:\n" << episodeArray << '\n';
   }
   */
   return 0;
}

