#pragma once

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <unordered_map>

inline void require_ro_crate_files_equal_by_id(
    const std::string& expectedPath,
    const std::string& actualPath
) {
    std::ifstream expectedFile(expectedPath);
    std::ifstream actualFile(actualPath);

    REQUIRE(expectedFile.good());
    REQUIRE(actualFile.good());

    const auto expected = nlohmann::json::parse(expectedFile);
    const auto actual = nlohmann::json::parse(actualFile);

    REQUIRE(expected.at("@context") == actual.at("@context"));

    std::unordered_map<std::string, nlohmann::json> expectedEntities;
    std::unordered_map<std::string, nlohmann::json> actualEntities;

    for (const auto& entity : expected.at("@graph")) {
        expectedEntities.emplace(entity.at("@id"), entity);
    }

    for (const auto& entity : actual.at("@graph")) {
        actualEntities.emplace(entity.at("@id"), entity);
    }

    REQUIRE(expectedEntities.size() == actualEntities.size());

    for (const auto& [id, expectedEntity] : expectedEntities) {
        REQUIRE(actualEntities.contains(id));
        REQUIRE(expectedEntity == actualEntities.at(id));
    }
}
