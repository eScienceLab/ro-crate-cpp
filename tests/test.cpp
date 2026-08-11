#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

using rocrate::ROCrate;
using rocrate::json;

namespace {

  const json& entity_by_id(const json& graph, const std::string& id) {
    for (const auto& e : graph)
      if (e.contains("@id") && e["@id"] == id)
        return e;
    throw std::runtime_error("no entity with @id '" + id + "'");
  }

} // namespace

TEST_CASE("RO-Crate creates a valid, minimal, RO-Crate json object")
{
    ROCrate crate;
    const json doc = crate.to_json();

    // Check there is a context entry referencing the RO-Crate 1.1 context
    REQUIRE(doc.contains("@context"));
    REQUIRE(doc["@context"].is_string());
    REQUIRE(doc["@context"] == "https://w3id.org/ro/crate/1.1/context");

    // Check there is a graph containing the metadata descriptor and the root
    REQUIRE(doc.contains("@graph"));
    REQUIRE(doc["@graph"].is_array());
    REQUIRE(doc["@graph"].size() == 2);

    const json& md = entity_by_id(doc["@graph"], "ro-crate-metadata.json");
    REQUIRE(md.contains("@type"));
    REQUIRE(md["@type"] == "CreativeWork");
    REQUIRE(md.contains("conformsTo"));
    REQUIRE(md["conformsTo"].is_object());
    REQUIRE(md["conformsTo"] == json{{"@id", "https://w3id.org/ro/crate/1.1"}});
    REQUIRE(md.contains("about"));
    REQUIRE(md["about"].is_object());
    REQUIRE(md["about"] == json{{"@id", "./"}});

    const json& root = entity_by_id(doc["@graph"], "./");
    REQUIRE(root.contains("@type"));
    REQUIRE(root["@type"] == "Dataset");
}

TEST_CASE("A dataset links to its files via hasPart")
{
    ROCrate crate;
    auto& dataset = crate.add("#dataset", {"Dataset"},
                              {{"name", "My dataset"},
                               {"description", "Example data"}});
    auto& file = crate.add("data/results.csv", {"File"},
                           {{"encodingFormat", "text/csv"}});
    dataset.add("hasPart", file);

    const json doc = crate.to_json();
    REQUIRE(doc["@graph"].size() == 4);

    const json& d = entity_by_id(doc["@graph"], "#dataset");
    REQUIRE(d["@type"] == "Dataset");
    REQUIRE(d["name"] == "My dataset");
    REQUIRE(d["description"] == "Example data");
    REQUIRE(d["hasPart"] == json::array({json{{"@id", "data/results.csv"}}}));

    const json& f = entity_by_id(doc["@graph"], "data/results.csv");
    REQUIRE(f["@type"] == "File");
    REQUIRE(f["encodingFormat"] == "text/csv");
}

TEST_CASE("write() produces a parseable RO-Crate document")
{
    ROCrate crate;
    auto& dataset = crate.add("#dataset", {"Dataset"}, {{"name", "My dataset"}});
    auto& file = crate.add("data/results.csv", {"File"});
    dataset.add("hasPart", file);

    const std::string path = "ro-crate-write-test.json";
    crate.write(path);

    std::ifstream in(path);
    REQUIRE(in.good());
    const json doc = json::parse(in);

    REQUIRE(doc == crate.to_json());
    REQUIRE(doc["@graph"].size() == 4);

    std::remove(path.c_str());
}
