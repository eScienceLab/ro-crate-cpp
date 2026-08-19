#include <catch2/catch_test_macros.hpp>

#include "test_helpers.hpp"
#include <ro-crate.hpp>

using rocrate::Entity;
using rocrate::ROCrate;

// https://www.researchobject.org/ro-crate/specification/1.1/root-data-entity#minimal-example-of-ro-crate
TEST_CASE("Minimal RO-Crate", "[integration]")
{
  ROCrate crate;

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("identifier", "https://doi.org/10.4225/59/59672c09f4a4b");
  rootData.set("datePublished", "2017");
  rootData.set("name", "Data files associated with the manuscript:Effects of facilitated family case conferencing for ...");
  rootData.set("description", "Palliative care planning for nursing home residents with advanced dementia ...");

  // Create the license entity
  Entity license({"CreativeWork"});
  license.set("description", "This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Australia License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/au/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.");
  license.set("identifier", "https://creativecommons.org/licenses/by-nc-sa/3.0/au/");
  license.set("name", "Attribution-NonCommercial-ShareAlike 3.0 Australia (CC BY-NC-SA 3.0 AU)");

  crate.addEntity("https://creativecommons.org/licenses/by-nc-sa/3.0/au/", license);

  // Add license to root data entity
  rootData.set("license", license);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/minimal-example-of-ro-crate.json",
    outputPath
  );
}

// https://www.researchobject.org/ro-crate/specification/1.1/appendix/jsonld
TEST_CASE("Example with file, author, location", "[integration]")
{
    ROCrate crate;

    // Add description to the root metadata entity (ro-crate-metadata.json)
    Entity root = crate.getEntity("ro-crate-metadata.json");
    root.set("description", "RO-Crate Metadata File Descriptor (this file)");
    REQUIRE_NOTHROW(crate.getEntity("ro-crate-metadata.json"));

    // Add name, description to the root data entity (./)
    Entity rootData = crate.getEntity("./");
    rootData.set("name", "Example RO-Crate");
    rootData.set("description", "The RO-Crate Root Data Entity");
    REQUIRE_NOTHROW(crate.getEntity("./"));

    // Create the person
    Entity alice({"Person"});
    alice.set("name", "Alice");
    alice.set("description", "One of hopefully many Contextual Entities");
    crate.addEntity("#alice", alice);
    REQUIRE_NOTHROW(crate.getEntity("#alice"));

    // Create the place
    Entity catalinaPark({"Place"});
    catalinaPark.set("name", "Catalina Park");
    crate.addEntity("http://sws.geonames.org/8152662/", catalinaPark);
    REQUIRE_NOTHROW(crate.getEntity("http://sws.geonames.org/8152662/"));

    // Create two datasets
    Entity data1({"File"});
    data1.set("description", "One of hopefully many Data Entities");
    data1.set("author", alice);
    data1.set("contentLocation", catalinaPark);
    crate.addEntity("data1.txt", data1);
    rootData.set("hasPart", data1);
    REQUIRE_NOTHROW(crate.getEntity("data1.txt"));

    Entity data2({"File"});
    crate.addEntity("data2.txt", data2);
    rootData.set("hasPart", data2);
    REQUIRE_NOTHROW(crate.getEntity("data2.txt"));

    // Write out
    const std::string outputPath =
        std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
    crate.writeOut(outputPath);

    REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
        std::string(TEST_SOURCE_DIR) +
            "/tests/fixtures/example-with-file-author-location.json",
        outputPath
    );
}

TEST_CASE("Example with web resources", "[integration]")
{
  ROCrate crate;

  // Survey responses file
  Entity surveyResponses({"File"});
  surveyResponses.set("name", "Survey responses");
  surveyResponses.set("contentSize", "26452");
  surveyResponses.set("encodingFormat", "text/csv");
  crate.addEntity("survey-responses-2019.csv", surveyResponses);

  // RO-Crate specification file
  Entity roCrateSpec({"File"});
  roCrateSpec.set("name", "RO-Crate specification");
  roCrateSpec.set("contentSize", "310691");
  roCrateSpec.set("description", "RO-Crate specification");
  roCrateSpec.set("encodingFormat", "application/pdf");
  crate.addEntity("https://zenodo.org/record/3541888/files/ro-crate-1.0.0.pdf", roCrateSpec);

  // Add to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("hasPart", surveyResponses);
  rootData.set("hasPart", roCrateSpec);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/example-with-web-resources.json",
    outputPath
  );
  
}
