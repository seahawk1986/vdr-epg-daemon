#include "tmdbactor.h"

cTMDBActor::cTMDBActor(const cJsonNode &data, const std::shared_ptr<sApiData> config): _config(config) {
        thumbUrl = _config->mediaBaseUrl + _config->profile_sizes.back() + data["profile_path"].stringValue();
        name = data["name"].stringValue();
        role = data["character"].stringValue();
        id = data["id"].intValue();
        sortOrder = data["order"].intValue();
        thumbUrlWidth = 300;
        thumbUrlHeight = 450;
}

void cTMDBActors::ParseJson(const string &jsonString, vActors &actors) {
    cJsonNode data = cJsonNode(jsonString);
    ParseJson(data, actors);
}

void cTMDBActors::ParseJson(const cJsonNode &data, vActors &actors) {
    actors.reserve(actors.size() + data.arraySize());
    for (const auto &actorData: data.ArrayToVector()) {
        actors.emplace_back(std::make_unique<cTMDBActor>(actorData, _config));
    }
}