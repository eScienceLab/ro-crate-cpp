#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

TEST_CASE("RO-Crate creates a valid, minimal, RO-Crate json object")
{
    ROCrate rocrate;

    // Check there is an object and that it is not empty
    REQUIRE(rocrate.crate().is_object());
    REQUIRE_FALSE(rocrate.crate().empty());

    // Check there is a context entity and that it contains an array which defaults to size 1
    REQUIRE(rocrate.crate().contains("@context"));
    REQUIRE(rocrate.crate()["@context"].is_array());
    REQUIRE(rocrate.crate()["@context"].size() == 1);
    REQUIRE(rocrate.crate()["@context"][0] == "https://w3id.org/ro/crate/1.1/context");

    // Check there is a graph entity
    REQUIRE(rocrate.crate().contains("@graph"));

    // Check that the graph contains the root data set
    REQUIRE(rocrate.crate()["@graph"].is_array());
    REQUIRE_FALSE(rocrate.crate()["@graph"].empty());
    
    REQUIRE(rocrate.crate()["@graph"][0].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][0]["@id"] == "ro-crate-metadata.json");
    REQUIRE(rocrate.crate()["@graph"][0].contains("@type"));
    REQUIRE(rocrate.crate()["@graph"][0]["@type"] == "CreativeWork");
    
    REQUIRE(rocrate.crate()["@graph"][0].contains("conformsTo"));
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"].is_array());
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"].size() == 1);
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"][0].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"][0]["@id"] == "https://w3id.org/ro/crate/1.1");
    
    REQUIRE(rocrate.crate()["@graph"][0].contains("about"));
    REQUIRE(rocrate.crate()["@graph"][0]["about"].is_array());
    REQUIRE(rocrate.crate()["@graph"][0]["about"].size() == 1);
    REQUIRE(rocrate.crate()["@graph"][0]["about"][0].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][0]["about"][0]["@id"] == "./");

    // Check the graph contains the root data entity
    REQUIRE(rocrate.crate()["@graph"].size() == 2);
    REQUIRE(rocrate.crate()["@graph"][1].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][1]["@id"] == "./");
    REQUIRE(rocrate.crate()["@graph"][1].contains("@type"));
    REQUIRE(rocrate.crate()["@graph"][1]["@type"] == "Dataset");
}
