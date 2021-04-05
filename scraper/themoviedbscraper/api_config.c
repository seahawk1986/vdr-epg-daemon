#include "api_config.h"

sApiData::sApiData(const std::string &ApiKey, const std::string &Language) : apiKey(ApiKey.empty() ? EpgdConfig.scrapMovieDbApiKey : ApiKey),
                                                                             language(Language),
                                                                             configurationUrl(baseUrl + "/3/configuration?api_key=" + apiKey),
                                                                             tvGenreUrl(baseUrl + "/3/genre/tv/list?api_key=" + apiKey + "&language=" + language)
{
    // LoadAPIConfig();
    // LoadTvGenres();
}

bool sApiData::LoadAPIConfig(void)
{
    std::string configJson;
    if (curl.GetUrl(configurationUrl.c_str(), &configJson))
    {
        auto data = cJsonNode(configJson);
        if (data.isNull())
            return false;
        
        mediaBaseUrl = data["images"]["base_url"].stringValue();
        data["images"]["backdrop_sizes"].addStringArrayToVector(backdrop_sizes);
        data["images"]["logo_sizes"].addStringArrayToVector(logo_sizes);
        data["images"]["poster_sizes"].addStringArrayToVector(poster_sizes);
        data["images"]["profile_sizes"].addStringArrayToVector(profile_sizes);
        data["images"]["still_sizes"].addStringArrayToVector(still_sizes);
        LoadTvGenres();
        return true;
    } else {
        std::cerr << "request for url " << configurationUrl << " failed" << "\n";
        return false;
    }
}

bool sApiData::LoadTvGenres(void)
{
    std::cerr << "GenreUrl: " << tvGenreUrl << "\n";
    std::string genresJson;
    if (curl.GetUrl(tvGenreUrl.c_str(), &genresJson))
    {
        auto data = cJsonNode(genresJson);
        if (data.isNull())
            return false;
        auto genre_array = data["genres"];
        // std::cout << json_dumps(genre_array.object(), JSON_INDENT(2)) << "\n";
        tv_genres.reserve(genre_array.arraySize());
        cJsonNode element;

        for (const auto &element : genre_array.ArrayToVector())
        {
            int id = element.intByName("id");
            auto name = element.stringByName("name");
            tv_genres[id] = name;
        }
        return true;
    }
    return false;
}