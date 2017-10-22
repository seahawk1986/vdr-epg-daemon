
#include <jansson.h>
#include "../../lib/curl.h"
#include "../../levenshtein.h"
#include "thetvdbscraper.h"

using namespace std;

cTVDBScraper::cTVDBScraper(string language) {
    apiKey = "E9DBB94CA50832ED";
    apiUrl = "https://api.thetvdb.com";
    this->language = language;
    // OLD API
    baseURL = "thetvdb.com";
    mirrors = NULL; // Obsolete
    xmlInitParser();
    // END OLD API
}

cTVDBScraper::~cTVDBScraper() {
    // OLD API
    if (mirrors)
        delete mirrors;
    // END OLD API
}

cTVDBSeries *cTVDBScraper::ScrapInitial(string seriesName) {
    cTVDBSeries *series = NULL;
    int seriesID = ReadSeries(seriesName);
    if (seriesID) {
        series = new cTVDBSeries(seriesID, language, apiKey, mirrors);
    }
    return series;    
}

cTVDBSeries *cTVDBScraper::GetSeries(int seriesID) {
    return new cTVDBSeries(seriesID, language, apiKey, mirrors); 
}

cTVDBEpisode *cTVDBScraper::GetEpisode(int episodeID) {
    return new cTVDBEpisode(episodeID, language, apiKey, mirrors); 
}

bool cTVDBScraper::Connect(void) {
    // Old API
    stringstream url;
    url << baseURL << "/api/" << apiKey << "/mirrors.xml";
    string mirrorsXML;
    bool ok = false;
    if (curl.GetUrl(url.str().c_str(), &mirrorsXML)) {
        mirrors = new cTVDBMirrors();
        ok = mirrors->ParseXML(mirrorsXML);
    }
    // END OLD API
    // New API
    ok = Login();
    return ok;
}

bool cTVDBScraper::Login(const std::string &apiKey)
{
   if (!apiKey.empty())
       this->apiKey = apiKey;

   string login_url = apiUrl + "/login";
   string login_data = "{\"apikey\": \"" + this->apiKey + "\"}";

   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");
   headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

   std::string data;
   long httpResponse = 0;
   int responseCode = curl.DoPost(login_url.c_str(), login_data, &data, httpResponse, headerlist);
   curl_slist_free_all(headerlist);

   if (responseCode) {
      if (httpResponse == 401) {
          cout << "Login failed" << '\n';
          return false;
      }
      return ParseToken(data);
   } else {
      cout << data << httpResponse << '\n' << data << '\n';
      return false;
   }
}

bool cTVDBScraper::UpdateToken(const std::string &token)
{
   if (!token.empty())
      this->token = token;

   const string url = apiUrl + "/refresh_token";
   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");
   string auth_token = "Authorization: Bearer " + this->token;
   headerlist = curl_slist_append(headerlist, auth_token.c_str());
   string referrer = "";

   string data = "";
   int res = curl.GetUrl(url.c_str(), &data, referrer, headerlist);
   curl_slist_free_all(headerlist);
   if (res) {
      // cout << "new token data: " << data << '\n';
      return ParseToken(data);
   } else {
      cout << "something went wrong: " << data << referrer << '\n';
      return false;
   }
}

bool cTVDBScraper::ParseToken(const string &s)
{
   json_t *data, *value;
   bool success = ParseJSON(s, &data);

   if (!success || !json_is_object(data)) {
      fprintf(stderr, "error: data structure is no object\n");
      return false;
   }

   value = json_object_get(data, "token");

   if (!json_is_string(value))
      return false;

   this->token = json_string_value(value);
   cout << "new Token is" << token << '\n';
   return true;
}

bool cTVDBScraper::ParseJSON(const string &data, json_t **result)
{
   json_error_t error;
   *result = json_loads(data.c_str(), 0, &error);

   if (!result) {
      fprintf(stderr, "error parsing json string on line %d: %s", error.line, error.text);
      return false;
   }

   return true;
}

int cTVDBScraper::GetServerTime(void) {
    // OLD API - TODO: Read time from header
    string url = "http://thetvdb.com/api/Updates.php?type=none";
    string serverTimeXML;
    if (!curl.GetUrl(url.c_str(), &serverTimeXML)) {
        return 0;
    }
    xmlDoc *doc = SetXMLDoc(serverTimeXML);
    if (doc == NULL)
        return 0;
    //Root Element has to be <Items>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Items"))) {
        xmlFreeDoc(doc);
        return 0;
    }
    node = node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Time")) {
            node = cur_node;
            break;
        } else {
            node = NULL;
        }
    }
    if (!node) {
        xmlFreeDoc(doc);
        return 0;
    }
    xmlChar *node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
    int serverTime = atoi((const char *)node_content);
    xmlFree(node_content);
    xmlFreeDoc(doc);
    // NEW API
    // TODO: parse header of http POST request
    return serverTime;
}

bool cTVDBScraper::GetUpdatedSeriesandEpisodes(set<int> *updatedSeries, set<int> *updatedEpisodes, int lastScrap) {
    // NEW API TODO:
    // * fetch updates for up to a week
    // * find series which could have been updates in between
    stringstream url;
    url << "http://thetvdb.com/api/Updates.php?type=all&time=" << lastScrap;
    string updatedXML;
    if (!curl.GetUrl(url.str().c_str(), &updatedXML)) {
        return false;
    }
    xmlDoc *doc = SetXMLDoc(updatedXML);
    if (doc == NULL)
        return false;
    //Root Element has to be <Items>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Items"))) {
        xmlFreeDoc(doc);
        return false;
    }

    xmlNode *cur_node = node->children;
    for (; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Series")) {
            xmlChar *node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            int seriesId = atoi((const char *)node_content);
            xmlFree(node_content);
            updatedSeries->insert(seriesId);
        } else if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Episode")) {
            xmlChar *node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            int episodeId = atoi((const char *)node_content);
            xmlFree(node_content);
            updatedEpisodes->insert(episodeId);
        }
    }
    xmlFreeDoc(doc);
    return true;
}

bool cTVDBScraper::GetRequest(const std::string &url, std::string &jsonString, const std::string &lang)
{
   // wrapper to handle get requests with the needed headers
   struct curl_slist *headerlist = NULL;
   headerlist = curl_slist_append(headerlist, "Accept: application/json");

   string lang_opt = "Accept-Language: " + language;
   headerlist = curl_slist_append(headerlist, lang_opt.c_str());

   string auth_token = "Authorization: Bearer " + token;
   headerlist = curl_slist_append(headerlist, auth_token.c_str());

   int res = curl.GetUrl(url.c_str(), &jsonString, (string)"", headerlist);
   curl_slist_free_all(headerlist);

   if (res) {
      // cout << "get request returned:\n" << jsonResult << '\n';
      return true;
   } else {
      return false;
   }
}

int cTVDBScraper::ReadSeries(const string &seriesName) {
   int seriesID = 0;
   // OLD API
   /*
   stringstream url;
   string seriesXML;
   char* escUrl = curl.EscapeUrl(seriesName.c_str());

   url << mirrors->GetMirrorXML() << "/api/GetSeries.php?seriesname=" << escUrl << "&language=" << language.c_str();
   curl.Free(escUrl);

   if (curl.GetUrl(url.str().c_str(), &seriesXML))
       seriesID = ParseXML(seriesXML);
   */
   // NEW API
   char* escName = curl.EscapeUrl(seriesName.c_str());
   const std::string url = apiUrl + "/search/series?name=" + escName;
   curl.Free(escName);

   std::string jsonString;

   if (!GetRequest(url.c_str(), jsonString, language))
      return seriesID;

   cout << "Search Series by name got Series data: " << jsonString << '\n';
   seriesID = GetSeriesID(jsonString, language);
   // TODO parse json string data
   return seriesID;
}

int cTVDBScraper::GetSeriesID(std::string &seriesData, string &seriesName)
{
   // return the series with the name nearest to seriesName
   int seriesID = 0;
   int lv_distance = 0;
   int max_dist = 0;
   json_auto_t *jsonSeriesData;
   bool success = ParseJSON(seriesData, &jsonSeriesData);
   const char *json_seriesName = NULL;
   json_auto_t *json_data, *json_array, *json_field, *json_seriesName_field;

   if (!json_is_object(jsonSeriesData)) {
      cout << "got no json object at top level!" << '\n';
      return seriesID;
   }

   json_array = json_object_get(jsonSeriesData, "data");

   if (!json_array) {
      cout << "error when fetching data" << '\n';
      return seriesID;
   }

   size_t index;
   json_auto_t *value;
   int bestMatch = 100;
   json_array_foreach(json_array, index, value) {
      json_data = json_array_get(json_array, index);

      if (!json_is_object(json_data)) {
         cout << "error when fetcing element of array" << '\n';
         continue;
      }

      json_seriesName_field = json_object_get(json_data, "seriesName");

      if (!json_seriesName_field) {
          cout << "error when fetching seriesName_field" << '\n';
          cout << "data was " << json_dumps(json_data, 0) << '\n';
      }

      json_seriesName = json_string_value(json_seriesName_field);
      if (!json_seriesName) {
         cout << "error when fetching seriesName" << '\n';
         continue;
      }

      lv_distance = lvDistance(seriesName, json_seriesName, 20, max_dist);
      cout << "Levenshtein distance between " << seriesName << " and " << json_seriesName << " is " << lv_distance << '\n';

      if (lv_distance < bestMatch) {
         bestMatch = lv_distance;

         json_field = json_object_get(json_data, "id");

         if (!json_field) {
            cout << "error when fetching id" << '\n';
            continue;
         }

         seriesID = json_integer_value(json_field);
      } 
   }
   return seriesID;
}

int cTVDBScraper::ParseXML(string xml) {
    int seriesID = 0;
    xmlDoc *doc = SetXMLDoc(xml);
    if (doc == NULL)
        return seriesID;
    //Root Element has to be <Data>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Data"))) {
        xmlFreeDoc(doc);
        return seriesID;
    }
    //Searching for  <Series>
    node = node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Series")) {
            node = cur_node;
            break;
        } else {
            node = NULL;
        }
    }
    if (!node) {
        xmlFreeDoc(doc);
        return seriesID;
    }
    //now read the first series    
    node = node->children;
    xmlChar *node_content;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            if (!node_content)
                continue;
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"seriesid")) {
                seriesID = atoi((const char *)node_content);
                xmlFree(node_content);
                break;
            }
            xmlFree(node_content);
        }
    }
    xmlFreeDoc(doc);
    return seriesID;
}

xmlDoc *cTVDBScraper::SetXMLDoc(string xml) {
    xmlDoc *doc = xmlReadMemory(xml.c_str(), xml.size(), "noname.xml", NULL, 0);
    return doc;
}
