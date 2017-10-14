#ifndef LOG_PREFIX
#define LOG_PREFIX "TEST"
#endif
const char* logPrefix = LOG_PREFIX;

bool parseToken(const string &s, string &token);
bool login(const string &apikey, string &token);
bool refreshToken(string &token);
int searchSeries(const string &token, const string &seriesName, const string &language="de");
//bool parseSeries(const string &seriesData, cTVDBSeries);