#include <string>
#include <vector>

#include <stdio.h>
#include <unistd.h>

#include "epgdconfig.h"
#include "levenshtein.h"

#include "scraper/themoviedbscraper/themoviedbscraper.h"

const char* logPrefix = "parsertest";
string language("DE");

typedef struct SearchResult {
    bool is_movie = false;
    bool is_tv = false;
    std::unique_ptr<cTMDBSeries> SeriesResult{};
    std::unique_ptr<cMovieDbMovie> MovieResult{};
    SearchResult(){};
} SearchResult;

std::unique_ptr<SearchResult> parse(const std::string name, bool is_series=false) {
    
    auto movieDbScraper = std::make_unique<cMovieDBScraper>(language);
    bool success = movieDbScraper->Connect();
    std::cerr << "connect was" << (success ? " " : " not ") << "successfull\n";
    auto result = std::make_unique<SearchResult>();
    auto movie_result = movieDbScraper->Scrap(name, std::string());
    if (movie_result) {
        result->MovieResult = std::unique_ptr<cMovieDbMovie>(movie_result);
        result->is_movie = true;
    }
    if (is_series) {
        // prioritise series results
    }
    return result;
}

int main(int argc, char *argv[]) {
    cEpgConfig::logstdout = yes;
    cEpgConfig::loglevel = 2;

    auto arguments = std::vector<std::string>(argv + 1, argv + argc);
    for (const auto &arg: arguments) {
        std::cout << arg << "\n";
        auto result = parse(arg);
        if (result->is_movie) {
            result->MovieResult->ReadActors();
            result->MovieResult->Dump();
        }
        if (result->is_tv) {
            result->SeriesResult->Dump(5);
        }
        // std::cout << (result->is_movie ? "true" : "false") << '\n';
    }
    return 0;
}