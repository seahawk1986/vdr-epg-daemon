#include "thetvdbapi.h"

using namespace std;

cTVDBApi::cTVDBApi(const string &apikey, const string &language) {
    this->apikey = apikey;
    this->language = language;
    cout << "APIKey: " << apikey << '\n';
    apiUrl = "https://api.thetvdb.com";
}

cTVDBApi::~cTVDBApi() {
}

bool cTVDBApi::Login(const std::string &apiKey)
{
   if (!apiKey.empty())
       this->apikey = apiKey;

   string login_url = apiUrl + "/login";
   std::cout << "URL is: " << login_url << ", apikey is: " << this->apikey << '\n';
   string login_data = "{\"apikey\": \"" + this->apikey + "\"}";

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

bool cTVDBApi::UpdateToken(const std::string &token)
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

bool cTVDBApi::ParseToken(const string &s)
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

bool cTVDBApi::ParseJSON(const string &data, json_t **result)
{
   json_error_t error;
   *result = json_loads(data.c_str(), 0, &error);

   if (!result) {
      fprintf(stderr, "error parsing json string on line %d: %s", error.line, error.text);
      return false;
   }

   return true;
}

int cTVDBApi::ReadSeries(const string &seriesName) {
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

int cTVDBApi::GetSeriesID(std::string &seriesData, string &seriesName)
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

bool cTVDBApi::GetRequest(const std::string &url, std::string &jsonString, const std::string &lang)
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
