#include <catch2/catch_test_macros.hpp>

#include <ro-crate.hpp>

#include <stdexcept>

using rocrate::Entity;
using rocrate::ROCrate;

TEST_CASE("Produce an example RO-Crate faithfully")
{
    ROCrate crate;

    // Add description to the root metadata entity (ro-crate-metadata.json) 
    Entity root = crate.getEntity("ro-crate-metadata.json");
    root.set("description", "RO-Crate Metadata File Descriptor (this file)");

    REQUIRE_NOTHROW(crate.getEntity("ro-crate-metadata.json"));
    REQUIRE_NOTHROW(crate.getEntity("./"));

    // Add name, description to the root data entity (./)
    Entity rootData = crate.getEntity("./");
    rootData.set("name", "Example RO-Crate");
    rootData.set("description", "The RO-Crate Root Data Entity");

    // Create the person
    Entity alice({"Person"});
    alice.set("name", "Alice");
    alice.set("description", "One of hopefully many Contextual Entities");
    crate.addEntity("#alice", alice);

    // Create the place
    Entity catalinaPark({"Place"});
    catalinaPark.set("name", "Catalina Park");
    crate.addEntity("http://sws.geonames.org/8152662/", catalinaPark);

    // Create two datasets
    Entity data1({"File"});
    data1.set("description", "One of hopefully many Data Entities");
    data1.set("author", alice);
    data1.set("contentLocation", catalinaPark);
    crate.addEntity("data1.txt", data1);

    Entity data2({"File"});
    REQUIRE_NOTHROW(crate.addEntity("data2.txt", data2));
}
    crate.addEntity("data2.txt", data2);
    REQUIRE_NOTHROW(crate.getEntity("data2.txt"));
}
