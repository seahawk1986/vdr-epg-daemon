#include "thetvdbapi.h"

using namespace std;

cTVDBApi::cTVDBApi() {
}

cTVDBApi::~cTVDBApi() {
}

bool cTVDBApi::Login(const std::string apiKey, const string language)
{
  if (apiKey.empty() || language.empty())
      return false;
   this->apikey = apiKey;
   this->language = language;

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

bool cTVDBApi::GetJSONRequest(const std::string &url, json_t **jsonData, const std::string &lang)
{
   string data;
   bool res = GetRequest(url, data, lang);

   if (!res) {
      cout << "GetJSONRequest: GetRequest failed." << '\n';
      return false;
   }

   res = ParseJSON(data, jsonData);
   if (!res) {
      cout << "ParseJSON failed." << '\n';
      return false;
   }

   return true;
}
