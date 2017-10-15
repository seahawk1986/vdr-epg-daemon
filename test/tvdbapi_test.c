#include <iostream>
#include <string>
#include <sstream>
#include <jansson.h>
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

bool login(const string &apikey, string &token)
{
   char login_url[] = "https://api.thetvdb.com/login";
   string login_data = "{\"apikey\": \"" + apikey + "\"}";
   int responseCode = 0;
   long httpResponse = 0;

   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");
   headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

   string data = "";
   responseCode = curl.DoPost(login_url, login_data, &data, httpResponse, (string)"", headerlist);
   if (responseCode) {
      return parseToken(data, token);
   } else {
      cout << data << httpResponse << '\n';
      return false;
   }
}

bool parseToken(const string &s, std::string &token)
{
   json_t *data, *value;
   json_error_t error;
   data = json_loads(s.c_str(), 0, &error);
   if (!data) {
      fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
      return false;
   }
   if (!json_is_object(data)) {
      fprintf(stderr, "error: data structure is no object\n");
      return false;
   }
   value = json_object_get(data, "token");
   if (!json_is_string(value))
      return false;
   token = json_string_value(value);
   return true;

}

bool refreshToken(string &token)
{
   string url = api_url + "/refresh_token";
   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");
   string auth_token = "Authorization: Bearer " + token;
   headerlist = curl_slist_append(headerlist, auth_token.c_str());
   string referrer = "";

   string data = "";
   if (curl.GetUrl(url.c_str(), &data, referrer, headerlist)) {
      // cout << "new token data: " << data << '\n';
      return parseToken(data, token);
   } else {
      cout << "something went wrong: " << data << referrer << '\n';
      return false;
   }

}

int searchSeries(const string &token, const string &seriesName, string &jsonResult, const string &language)
{
   int seriesID = 0;
   string seriesJSON;
   stringstream url;

   char* escName = curl.EscapeUrl(seriesName.c_str());
   url << api_url << "/search/series?name=" << escName;
   curl.Free(escName);

   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");

   string lang_opt = "Accept-Language: " + language;
   headerlist = curl_slist_append(headerlist, lang_opt.c_str());

   string auth_token = "Authorization: Bearer " + token;
   headerlist = curl_slist_append(headerlist, auth_token.c_str());

   if (curl.GetUrl(url.str().c_str(), &seriesJSON, (string)"", headerlist)) {
      //parseSeriesJSON(seriesJSON);
      cout << "Series data: " << seriesJSON << '\n';
                jsonResult = seriesJSON;
      return true;
   } else {
      return false;
   }
}

bool getSeries(const string &token, int seriesID, string &jsonResult, const string &language)
{
   string seriesJSON;
   stringstream url;
   url << api_url << "/series/" << seriesID;

   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");

   string lang_opt = "Accept-Language: " + language;
   headerlist = curl_slist_append(headerlist, lang_opt.c_str());

   string auth_token = "Authorization: Bearer " + token;
   headerlist = curl_slist_append(headerlist, auth_token.c_str());

   if (curl.GetUrl(url.str().c_str(), &seriesJSON, (string)"", headerlist)) {
      //parseSeriesJSON(seriesJSON);
      cout << "Series data: " << seriesJSON << '\n';
                jsonResult = seriesJSON;
      return true;
   } else {
      return false;
   }
}

int getSeriesID(json_t **seriesData, string &seriesName)
{
   int seriesID = 0;
        json_t *json_data, *json_array, *json_field;
        if (!json_is_object(*seriesData)) {
      cout << "got no json object at top level!" << '\n';
                json_decref(*seriesData);
      return seriesID;
   }
   json_array = json_object_get(*seriesData, "data");
        if (!json_array) {
      cout << "error when fetching data" << '\n';
                json_decref(*seriesData);
      return seriesID;
   }
        json_data = json_array_get(json_array, 0);
        if (!json_is_object(json_data)) {
      cout << "error when fetcing first element of array" << '\n';
                json_decref(*seriesData);
      return seriesID;
   }
   json_field = json_object_get(json_data, "id");
        if (!json_field) {
      cout << "error when fetching id" << '\n';
                json_decref(*seriesData);
      return seriesID;
   }
        seriesID = json_integer_value(json_field);
        /* TODO: return the best match for the original search string
   */
        return seriesID;

}

int main(int argc, char *argv[])
{
   string token = "";
   string apikey = "E9DBB94CA50832ED";
   if (login(apikey, token)) {
      cout << "token: " << token << "\n";
   } else {
      cout << "login failed" << std::endl;
   }
   if (refreshToken(token))
      cout << "new token: " << token << '\n';


   if (argc > 1) {
      searchArg = argv[1];
      std::cout << "argument was: " << searchArg << std::endl;
   } else {
      searchArg = "Mit Schirm, Charme und Melone";
   }
        string jsonData;
   if (!searchSeries(token, searchArg, jsonData)) {
      cout << "search failed" << '\n';
      return 1;
   }
        json_t *root;
        cout << "Parsing json string:" << '\n' << jsonData << '\n';
        bool success = parseJSON(jsonData, &root);
        if (!success) {
      cout << "error when parsing JSON string" << '\n';
      return 1;
   }
   int seriesID;
   seriesID = getSeriesID(&root, searchArg);
        json_decref(root);
   cout << "SeriesID for " << searchArg << " is " << seriesID << '\n';
   getSeries(token, seriesID, jsonData);
   return 0;
}

