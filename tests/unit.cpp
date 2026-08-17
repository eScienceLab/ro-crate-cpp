#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

#include <stdexcept>

using rocrate::Entity;

TEST_CASE("Ensure failure if entity is linked before being assigned an ID", "[unit]")
{
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
