#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

#include <stdexcept>

using rocrate::Entity;

// ------------------------------------------------------------------------------
// Unit tests for Entity class
// ------------------------------------------------------------------------------

TEST_CASE("Ensure entity creation occurs without throwing", "[unit]") {
    Entity alice({"Person"});
}

TEST_CASE("Ensure entity creation throws if no types are provided", "[unit]") {
  // Create an entity with an empty types vector, which should throw an invalid_argument exception
  REQUIRE_THROWS_AS(Entity(std::vector<std::string>{}), std::invalid_argument);
}

TEST_CASE("Ensure set by value succeeds with valid input", "[unit]") {
    Entity alice({"Person"});
    REQUIRE_NOTHROW(alice.set("name", "Alice"));
    REQUIRE_NOTHROW(alice.set("description", "One of hopefully many Contextual Entities"));
}

TEST_CASE("Ensure failure if entity is linked before being assigned an ID", "[unit]") {
    // Create the person, but do not add to a crate (so no @id yet)
    Entity alice({"Person"});
    alice.set("name", "Alice");
    alice.set("description", "One of hopefully many Contextual Entities");

    // Create a dataset and attempt to link to the person
    Entity data1({"File"});
    data1.set("description", "One of hopefully many Data Entities");

    // This fails at link time because the linked entity has no @id yet.
    REQUIRE_THROWS_AS(data1.set("author", alice), std::runtime_error);
}

TEST_CASE("Ensure set by entity succeeds with valid input", "[unit]") {
  // This test is exercised by the integration test as it requires a crate
  SUCCEED("");
}

TEST_CASE("Add entity rejects entities with duplicate IDs", "[unit]") {
  // Create a crate and add an entity
  rocrate::ROCrate crate;

  Entity alice({"Person"});
  alice.set("name", "Alice");
  crate.addEntity("#alice", alice);

  // Attempt to add another entity with the same ID
  Entity aliceDuplicate({"Person"});
  aliceDuplicate.set("name", "Alice Duplicate");
  REQUIRE_THROWS_AS(crate.addEntity("#alice", aliceDuplicate), std::runtime_error);
}

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
