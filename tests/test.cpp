#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

TEST_CASE("RO-Crate creates a valid, minimal, RO-Crate json object")
{
    ROCrate rocrate;

    // Check there is an object and that it is not empty
    REQUIRE(rocrate.crate().is_object());
    REQUIRE_FALSE(rocrate.crate().empty());

    // Check there is a context entry referencing the RO-Crate 1.1 context
    REQUIRE(rocrate.crate().contains("@context"));
    REQUIRE(rocrate.crate()["@context"].is_string());
    REQUIRE(rocrate.crate()["@context"] == "https://w3id.org/ro/crate/1.1/context");

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
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"].is_object());
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][0]["conformsTo"]["@id"] == "https://w3id.org/ro/crate/1.1");
    
    REQUIRE(rocrate.crate()["@graph"][0].contains("about"));
    REQUIRE(rocrate.crate()["@graph"][0]["about"].is_object());
    REQUIRE(rocrate.crate()["@graph"][0]["about"].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][0]["about"]["@id"] == "./");

    // Check the graph contains the root data entity
    REQUIRE(rocrate.crate()["@graph"].size() == 2);
    REQUIRE(rocrate.crate()["@graph"][1].contains("@id"));
    REQUIRE(rocrate.crate()["@graph"][1]["@id"] == "./");
    REQUIRE(rocrate.crate()["@graph"][1].contains("@type"));
    REQUIRE(rocrate.crate()["@graph"][1]["@type"] == "Dataset");
}
