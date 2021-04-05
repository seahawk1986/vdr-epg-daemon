
#ifndef __TVSCRAPER_TVDBMANAGER_H
#define __TVSCRAPER_TVDBMANAGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "lib/common.h"
#include "lib/db.h"
#include "lib/epgservice.h"

#include "epgdconfig.h"

#include "scraper/themoviedbscraper/thetmdbscraper.h"
#include "scraper/themoviedbscraper/tmdbseries.h"
#include "scraper/themoviedbscraper/tmdbepisode.h"
#include "scraper/themoviedbscraper/tmdbmedia.h"
#include "scraper/themoviedbscraper/tmdbactor.h"

struct sSeriesResult {
    tEventId eventId;
    string title;
    int lastScraped;
    int season;
    int part;
    int number;
    string episodeName;  
};

// --- cTVDBManager -------------------------------------------------------------

class cTVDBManager {
private:
    std::shared_ptr<sApiData> config;
    int bytesDownloaded;
    int withutf8;
    string language;
    int serverTime;
    map<string, int> alreadyScraped;
    std::unique_ptr<cTMDBScraper> tvdbScraper = nullptr;
    cDbConnection *connection;
    std::unique_ptr<cDbTable> tSeries;
    std::unique_ptr<cDbTable> tSeriesEpsiode;
    std::unique_ptr<cDbTable> tSeriesMedia;
    std::unique_ptr<cDbTable> tSeriesActor;
    std::unique_ptr<cDbTable> tEvents;
    std::unique_ptr<cDbTable> tEpisodes;
    std::unique_ptr<cDbTable> tRecordingList;

    enum mediaType {
         mtBanner1,
         mtBanner2,
         mtBanner3,
         mtPoster1,
         mtPoster2,
         mtPoster3,
         mtSeasonPoster,
         mtFanart1,
         mtFanart2,
         mtFanart3,
         mtEpisodePic,
         mtActorThumb
    };
    int GetLastScrap(void);
    void UpdateScrapTS(void);
    bool GetAllIDs(set<std::pair<int, int>> &IDs, std::unique_ptr<cDbTable> &table, const char* fname, const char* sname);
    bool GetAllIDs(set<int> &IDs, std::unique_ptr<cDbTable> &table, const char* fname);
    std::unique_ptr<cTMDBSeries> ScrapSeries(string search);
    std::unique_ptr<cTMDBSeries> ScrapSeries(int seriesID);
    void SaveSeries(std::unique_ptr<cTMDBSeries> &series);
    void SaveSeriesBasics(std::unique_ptr<cTMDBSeries> &series);
    void SaveSeriesMedia(std::unique_ptr<cTMDBSeries> &series);
    template <typename T>
    bool SavePosterBannerFanart(int mediaType, const std::unique_ptr<T> &media, int seriesID);
    void SaveSeriesEpisodes(const std::unique_ptr<cTMDBSeries> &series);
    void SaveSeriesEpisode(const std::unique_ptr<cTMDBEpisode> &episode, int seriesID = 0);
    void SaveSeriesActors(const std::unique_ptr<cTMDBSeries> &series);
    bool LoadMedia(int seriesId, int seasonNumber, int episodeId, int actorId, int mediaType);
    bool ReloadImage(string url);
    int GetPicture(const char* url, MemoryStruct* data);
    int LoadSeriesFromDB(string name);
    void GetSeasonEpisodeFromEpisodename(int seriesID, int &season, int &part, string episodeName);
    void LoadSeasonPoster(int seriesID, int season);
    int LoadEpisodePicture(int seriesID, int season, int part);
    void UpdateEvent(tEventId eventID, int seriesID, int episodeID);
    void DeleteSeries(int seriesId);
    int LoadEpisode(string name, int seriesId);
public:
    cTVDBManager(void);
    virtual ~cTVDBManager(void);
    int ConnectDatabase(cDbConnection *conn);
    bool ConnectScraper(void);
    void SetServerTime(void);
    void ResetBytesDownloaded(void) { bytesDownloaded = 0; };
    int GetBytesDownloaded(void) { return bytesDownloaded; };
    void UpdateSeries(void);
    bool GetSeriesWithEpisodesFromEPG(vector<sSeriesResult> *result);
    void ProcessSeries(sSeriesResult ser);
    int CleanupSeries(void);
    bool SearchRecordingDB(string name, string episode, int &seriesId, int &episodeId);
    bool SearchRecordingOnline(string name, string episode, int &seriesId, int &episodeId);
    bool CheckScrapInfoDB(int scrapSeriesId, int scrapEpisodeId);
    bool CheckScrapInfoOnline(int scrapSeriesId, int scrapEpisodeId);
};

#endif //__TVSCRAPER_TVDBMANAGER_H
