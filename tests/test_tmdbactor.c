#include <iostream>
#include <vector>
#include <memory>
#include <jansson.h>
#include "../tools/stringhelpers.h"
#include "../scraper/themoviedbscraper/tmdbactor.h"

const char* logPrefix = "test_tmdbactor.c";
using vActors = std::vector<std::unique_ptr<cTMDBActor>>;

const char* jsonData = R"json(
[
    {
      "adult": false,
      "gender": 1,
      "id": 1223786,
      "known_for_department": "Acting",
      "name": "Emilia Clarke",
      "original_name": "Emilia Clarke",
      "popularity": 14.322,
      "profile_path": "/r6i4C3kYrBRzUzZ8JKAHYQ0T0dD.jpg",
      "character": "Daenerys Targaryen",
      "credit_id": "5256c8af19c2956ff60479f6",
      "order": 0
    },
    {
      "adult": false,
      "gender": 2,
      "id": 13633,
      "known_for_department": "Acting",
      "name": "Mark Addy",
      "original_name": "Mark Addy",
      "popularity": 6.012,
      "profile_path": "/4ao6jitEAxmQDlJheEOKT8lhJI8.jpg",
      "character": "Robert Baratheon",
      "credit_id": "5256c8ad19c2956ff60478e2",
      "order": 1
    },
    {
      "adult": false,
      "gender": 1,
      "id": 17286,
      "known_for_department": "Acting",
      "name": "Lena Headey",
      "original_name": "Lena Headey",
      "popularity": 17.409,
      "profile_path": "/i2fAeyUY42URnR0bR4wBYvAo2eB.jpg",
      "character": "Cersei Lannister",
      "credit_id": "5256c8ad19c2956ff60479ce",
      "order": 1
    },
    {
      "adult": false,
      "gender": 1,
      "id": 1001657,
      "known_for_department": "Acting",
      "name": "Sophie Turner",
      "original_name": "Sophie Turner",
      "popularity": 10.289,
      "profile_path": "/1hUAKYvSi4vZSYvTnD2TlqF6VVx.jpg",
      "character": "Sansa Stark",
      "credit_id": "5256c8b419c2956ff6047f34",
      "order": 2
    },
    {
      "adult": false,
      "gender": 2,
      "id": 239019,
      "known_for_department": "Acting",
      "name": "Kit Harington",
      "original_name": "Kit Harington",
      "popularity": 4.84,
      "profile_path": "/4MqUjb1SYrzHmFSyGiXnlZWLvBs.jpg",
      "character": "Jon Snow",
      "credit_id": "5256c8af19c2956ff6047af6",
      "order": 3
    },
    {
      "adult": false,
      "gender": 2,
      "id": 22970,
      "known_for_department": "Acting",
      "name": "Peter Dinklage",
      "original_name": "Peter Dinklage",
      "popularity": 6.646,
      "profile_path": "/aVHPYuRNQub8aQYX9clFEdq9ZQS.jpg",
      "character": "Tyrion Lannister",
      "credit_id": "5256c8b219c2956ff6047cd8",
      "order": 4
    },
    {
      "adult": false,
      "gender": 2,
      "id": 12795,
      "known_for_department": "Acting",
      "name": "Nikolaj Coster-Waldau",
      "original_name": "Nikolaj Coster-Waldau",
      "popularity": 8.532,
      "profile_path": "/dv1zRmSvSg8lDrxeM0SswYze6Z0.jpg",
      "character": "Jaime Lannister",
      "credit_id": "5256c8ad19c2956ff604793e",
      "order": 5
    },
    {
      "adult": false,
      "gender": 1,
      "id": 1181313,
      "known_for_department": "Acting",
      "name": "Maisie Williams",
      "original_name": "Maisie Williams",
      "popularity": 26.361,
      "profile_path": "/zLTq39cdRjS43dEzb78c1p1QcbT.jpg",
      "character": "Arya Stark",
      "credit_id": "5256c8b419c2956ff6047f0c",
      "order": 6
    },
    {
      "adult": false,
      "gender": 2,
      "id": 489467,
      "known_for_department": "Acting",
      "name": "Jack Gleeson",
      "original_name": "Jack Gleeson",
      "popularity": 1.929,
      "profile_path": "/t29CnCe6UKonBAxwNXkpJ54QLYR.jpg",
      "character": "Joffrey Baratheon",
      "credit_id": "5256c8b119c2956ff6047c4e",
      "order": 7
    },
    {
      "adult": false,
      "gender": 2,
      "id": 84423,
      "known_for_department": "Acting",
      "name": "Conleth Hill",
      "original_name": "Conleth Hill",
      "popularity": 4.646,
      "profile_path": "/eeTnNiustUbShHU09EQ6LoOpzcg.jpg",
      "character": "Varys",
      "credit_id": "5256c8b219c2956ff6047d6e",
      "order": 9
    },
    {
      "adult": false,
      "gender": 2,
      "id": 239020,
      "known_for_department": "Acting",
      "name": "Isaac Hempstead-Wright",
      "original_name": "Isaac Hempstead-Wright",
      "popularity": 2.796,
      "profile_path": "/b6hQvID3oXPXnyrrXs22MBv2lyN.jpg",
      "character": "Bran Stark",
      "credit_id": "5256c8b119c2956ff6047c22",
      "order": 11
    },
    {
      "adult": false,
      "gender": 2,
      "id": 49735,
      "known_for_department": "Acting",
      "name": "Aidan Gillen",
      "original_name": "Aidan Gillen",
      "popularity": 6.389,
      "profile_path": "/ju5ho6nnwOQ2QLGLnDP9yOZhGpb.jpg",
      "character": "Petyr Baelish",
      "credit_id": "5256c8af19c2956ff6047aa4",
      "order": 12
    },
    {
      "adult": false,
      "gender": 2,
      "id": 3075,
      "known_for_department": "Acting",
      "name": "Rory McCann",
      "original_name": "Rory McCann",
      "popularity": 2.825,
      "profile_path": "/meEHyiCRXTTCiYQMzP4VEdvEuD0.jpg",
      "character": "Sandor Clegane",
      "credit_id": "5256c8b119c2956ff6047c84",
      "order": 14
    },
    {
      "adult": false,
      "gender": 2,
      "id": 205258,
      "known_for_department": "Acting",
      "name": "Harry Lloyd",
      "original_name": "Harry Lloyd",
      "popularity": 3.753,
      "profile_path": "/qZSf6OzRpDaZdOCX6pynSRp6jVV.jpg",
      "character": "Viserys Targaryen",
      "credit_id": "5256c8af19c2956ff6047ac2",
      "order": 26
    },
    {
      "adult": false,
      "gender": 2,
      "id": 512991,
      "known_for_department": "Acting",
      "name": "Richard Madden",
      "original_name": "Richard Madden",
      "popularity": 6.545,
      "profile_path": "/kC7X9LgAtJfpxUBRtVwaVTEXomH.jpg",
      "character": "Robb Stark",
      "credit_id": "5256c8af19c2956ff6047b1a",
      "order": 27
    },
    {
      "adult": false,
      "gender": 2,
      "id": 20508,
      "known_for_department": "Acting",
      "name": "Iain Glen",
      "original_name": "Iain Glen",
      "popularity": 5.771,
      "profile_path": "/vYEI5xJWJ6HKjPusvO2klAvez3J.jpg",
      "character": "Jorah Mormont",
      "credit_id": "5256c8af19c2956ff6047a5c",
      "order": 476
    },
    {
      "adult": false,
      "gender": 1,
      "id": 20057,
      "known_for_department": "Acting",
      "name": "Michelle Fairley",
      "original_name": "Michelle Fairley",
      "popularity": 5.555,
      "profile_path": "/j8lQfEPHCKWV2QCiJJuH97CnoBX.jpg",
      "character": "Catelyn Stark",
      "credit_id": "5256c8ad19c2956ff604796a",
      "order": 478
    },
    {
      "adult": false,
      "gender": 2,
      "id": 71586,
      "known_for_department": "Acting",
      "name": "Alfie Allen",
      "original_name": "Alfie Allen",
      "popularity": 4.228,
      "profile_path": "/zmhO4Awjf7naHPXkbJWwjQpojwq.jpg",
      "character": "Theon Greyjoy",
      "credit_id": "5256c8b019c2956ff6047b5a",
      "order": 478
    },
    {
      "adult": false,
      "gender": 2,
      "id": 48,
      "known_for_department": "Acting",
      "name": "Sean Bean",
      "original_name": "Sean Bean",
      "popularity": 22.671,
      "profile_path": "/kTjiABk3TJ3yI0Cto5RsvyT6V3o.jpg",
      "character": "Ned Stark",
      "credit_id": "58c7134792514179d20011a9",
      "order": 500
    },
    {
      "adult": false,
      "gender": 2,
      "id": 117642,
      "known_for_department": "Acting",
      "name": "Jason Momoa",
      "original_name": "Jason Momoa",
      "popularity": 9.196,
      "profile_path": "/6dEFBpZH8C8OijsynkSajQT99Pb.jpg",
      "character": "Drogo",
      "credit_id": "5d075ca49251416c7db8cf5d",
      "order": 500
    }
])json";

int main() {
    vActors actorsList;
    cTMDBActors actors("de", "https://foo");
    actors.ParseJson(jsonData, actorsList);
    for (const auto& actor: actorsList) {
        actor->Dump();
    }
    return 0;
}