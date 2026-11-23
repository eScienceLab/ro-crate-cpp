#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

#include <stdexcept>

#include "test_helpers.hpp"

using rocrate::Entity;
using rocrate::ROCrate;

// ------------------------------------------------------------------------------
// Unit tests for Entity class
// ------------------------------------------------------------------------------

TEST_CASE("Ensure entity creation occurs without throwing", "[unit]") {
    ROCrate crate;
    crate.addEntity("#alice", {"Person"});
}

TEST_CASE("Ensure entity creation throws if no types are provided", "[unit]") {
  // Adding an entity with an empty types vector should throw an invalid_argument exception
  ROCrate crate;
  REQUIRE_THROWS_AS(crate.addEntity("#thing", std::vector<std::string>{}), std::invalid_argument);
}

TEST_CASE("Ensure set by value succeeds with valid input", "[unit]") {
    ROCrate crate;
    Entity& alice = crate.addEntity("#alice", {"Person"});
    REQUIRE_NOTHROW(alice.set("name", "Alice"));
    REQUIRE_NOTHROW(alice.set("description", "One of hopefully many Contextual Entities"));
}

TEST_CASE("Rejects empty property names", "[unit]") {
    ROCrate crate;
    Entity& entity = crate.addEntity("#thing", {"Thing"});

    REQUIRE_THROWS_AS(entity.set("", "value"), std::invalid_argument);
}

TEST_CASE("Rejects empty property values", "[unit]") {
    ROCrate crate;
    Entity& entity = crate.addEntity("#thing", {"Thing"});

    REQUIRE_THROWS_AS(entity.set("name", ""), std::invalid_argument);
}

TEST_CASE("Rejects direct ID assignment", "[unit]") {
    ROCrate crate;
    Entity& entity = crate.addEntity("#thing", {"Thing"});

    REQUIRE_THROWS_AS(entity.set("@id", "#thing"), std::invalid_argument);
}

TEST_CASE("Rejects empty entity IDs", "[unit]") {
    ROCrate crate;

    REQUIRE_THROWS_AS(
        crate.addEntity("", {"Thing"}),
        std::invalid_argument
    );
}

TEST_CASE("Rejects empty link property names", "[unit]") {
    ROCrate crate;
    Entity& source = crate.addEntity("#source", {"Thing"});
    Entity& target = crate.addEntity("#target", {"Thing"});

    REQUIRE_THROWS_AS(
        target.set("", source),
        std::invalid_argument
    );
}

TEST_CASE("Ignore duplicate key-value pairs", "[unit]") {
    ROCrate crate;
    Entity& entity = crate.addEntity("#thing", {"Thing"});

    REQUIRE_NOTHROW(entity.set("name", "value"));
    REQUIRE_NOTHROW(entity.set("name", "value")); // Should not throw, but also not add a duplicate

    SUCCEED("No exceptions thrown, can't currently assert on entity values as they are not exposed in the API");
}

// -------------------------------------------------------------------------------
// Unit tests for RO-Crate class
// -------------------------------------------------------------------------------

TEST_CASE("Get entity throws if entity not found", "[unit]") {
  // Create a crate 
  rocrate::ROCrate crate;

  // Attempt to get an entity that does not exist
  REQUIRE_THROWS_AS(crate.getEntity("#bob"), std::runtime_error);
}

TEST_CASE("RO-Crate initilises with valid root metadata and dataset entities", "[unit]") {
  // Create a crate
  rocrate::ROCrate crate;
  
  // Check that the root metadata entity exists
  REQUIRE_NOTHROW(crate.getEntity("ro-crate-metadata.json"));
  
  // Check that the root dataset entity exists
  REQUIRE_NOTHROW(crate.getEntity("./"));
}

TEST_CASE("addEntity returns a mutable reference into the crate", "[unit]") {
  rocrate::ROCrate crate;
  Entity& alice = crate.addEntity("#alice", {"Person"});

  alice.set("name", "Alice");
  crate.getEntity("#alice").set("description", "Updated");

  SUCCEED("No exceptions thrown, can't currently assert on entity values as they are not exposed in the API");
}

TEST_CASE("Context can be added without error", "[unit]") {
  rocrate::ROCrate crate;

  REQUIRE_NOTHROW(crate.addContext("example", "https://example.com/context.jsonld"));
}

TEST_CASE("Duplicate context doesn't throw", "[unit]") {
  rocrate::ROCrate crate;

  REQUIRE_NOTHROW(crate.addContext("example", "https://example.com/context.jsonld"));
  REQUIRE_NOTHROW(crate.addContext("example", "https://example.com/context.jsonld"));
  
}

TEST_CASE("Add context throws if context value is empty", "[unit]") {
  rocrate::ROCrate crate;
  REQUIRE_THROWS_AS(crate.addContext("example", ""), std::invalid_argument);
}

TEST_CASE("Add context with and without prefix (key) yeilds correct JSON-LD output", "[unit]") {
  rocrate::ROCrate crate;

  // Add context with prefix
  crate.addContext("example", "https://example.com/context.jsonld");

  // Add context without prefix
  crate.addContext("", "https://example.com/context2.jsonld");

  // Roundtrip the crate to JSON-LD and back to ensure the contexts are preserved
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/add-context.out.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) + "/tests/fixtures/add-context.json",
    outputPath
  );
}

TEST_CASE("Add entity throws if ID is empty", "[unit]") {
  rocrate::ROCrate crate;

  REQUIRE_THROWS_AS(crate.addEntity("", {"Person"}), std::invalid_argument);
}

TEST_CASE("Add entity rejects entities with duplicate IDs", "[unit]") {
  // Create a crate and add an entity
  rocrate::ROCrate crate;

  Entity& alice = crate.addEntity("#alice", {"Person"});
  alice.set("name", "Alice");

  // Attempt to add another entity with the same ID
  REQUIRE_THROWS_AS(crate.addEntity("#alice", {"Person"}), std::runtime_error);
}

TEST_CASE("Write out RO-Crate to JSON-LD without throwing", "[unit]") {
  rocrate::ROCrate crate;

  Entity& alice = crate.addEntity("#alice", {"Person"});
  alice.set("name", "Alice");

  REQUIRE_NOTHROW(crate.writeOut("ro-crate-metadata.json"));
}

TEST_CASE("Read in RO-Crate from JSON-LD without throwing", "[unit]") {
  rocrate::ROCrate crate;

  REQUIRE_NOTHROW(
    crate.readIn(
      std::string(TEST_SOURCE_DIR) + "/tests/fixtures/minimal-example-of-ro-crate.json"
    )
  );
}

TEST_CASE("Read in RO-Crate with an entity typed with an array of types", "[unit]") {
  rocrate::ROCrate crate;

  REQUIRE_NOTHROW(
    crate.readIn(
      std::string(TEST_SOURCE_DIR) + "/tests/fixtures/entity-with-multiple-types.json"
    )
  );

  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/entity-with-multiple-types.out.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) + "/tests/fixtures/entity-with-multiple-types.json",
    outputPath
  );
}
