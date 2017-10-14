#include <iostream>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../lib/curl.h"
#include "../tools/stringhelpers.h"
#include "tvdbapi_test.h"

const string api_url = "https://api.thetvdb.com";
string searchArg;

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

int searchSeries(const string &token, const string &seriesName, const string &language)
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
		return true;
	} else {
		return false;
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		searchArg = argv[1];
		std::cout << "argument was: " << searchArg << std::endl;
	}
	string token = "";
	string apikey = "E9DBB94CA50832ED";
	if (login(apikey, token)) {
		cout << "token: " << token << "\n";
	} else {
		cout << "login failed" << std::endl;
	}
	if (refreshToken(token))
		cout << "new token: " << token << '\n';


	if (searchSeries(token, "Mit Schirm, Charme und Melone")) {
		cout << "successfull search" << '\n';
	} else {
		cout << "search failed" << '\n';
	}
	return 0;
}

