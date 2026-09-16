#include <catch2/catch_test_macros.hpp>

#include "test_helpers.hpp"
#include <ro-crate.hpp>

using rocrate::Entity;
using rocrate::ROCrate;

// REF: https://www.researchobject.org/ro-crate/specification/1.3/root-data-entity.html#minimal-example-of-ro-crate
// FIXTURE: tests/fixtures/minimal-example-of-ro-crate.json
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

// REF: https://www.researchobject.org/ro-crate/specification/1.3/data-entities.html#example-attached-ro-crate-package
// FIXTURE: tests/fixtures/example-with-file-directory.json
TEST_CASE("Example with file and directory", "[integration]")
{
  ROCrate crate;

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("name", "Example Dataset");
  rootData.set("datePublished", "2016-02-01");
  rootData.set("license", "CC-BY");

  // Create the author entity
  Entity author({"Person"});
  author.set("name", "Michael Lake");
  crate.addEntity("https://orcid.org/0000-0003-4953-0830", author);

  // Create the file entity
  Entity file({"File"});
  file.set("name", "Diagram showing trend to increase");
  file.set("contentSize", "383766");
  file.set("description", "Illustrator file for Glop Pot");
  file.set("encodingFormat", "application/pdf");
  crate.addEntity("cp7glop.ai", file);

  // Create the directory entity
  Entity directory({"Dataset"});
  directory.set("name", "Too many files");
  directory.set("description", "This directory contains many small files -- the name of the file is a date in YYYY-MM-DD.csv, each file contains daily temperature readings, sampled hourly for the Glop Pot cave.");
  crate.addEntity("lots_of_little_files/", directory);

  // Add file and directory to root data entity
  rootData.set("hasPart", file);
  rootData.set("hasPart", directory);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/example-with-file-directory.json",
    outputPath
  );
  
}

// REF: https://www.researchobject.org/ro-crate/specification/1.3/data-entities.html#web-based-data-entities
// FIXTURE: tests/fixtures/example-with-web-resources.json
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

// REF: https://www.researchobject.org/ro-crate/specification/1.3/crate-focus.html (RO-Crates with a data entity as mainEntity)
// FIXTURE: tests/fixtures/ro-crate-with-a-data-entity-as-mainentity.json
TEST_CASE("RO-Crate with a data entity as mainEntity", "[integration]")
{
  ROCrate crate;

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("name", "Example Workflow");
  rootData.set("description", "An example workflow RO Crate");
  rootData.set("license", "Apache-2.0");
  rootData.set("datePublished", "2023-01-01");

  // Create the mainEntity file entity
  Entity mainEntity({"File"});
  mainEntity.set("name", "example_workflow.cwl");
  crate.addEntity("example_workflow.cwl", mainEntity);

  // Set mainEntity for root data entity
  rootData.set("mainEntity", mainEntity);

  // Create additional file entities
  Entity diagram({"File"});
  diagram.set("name", "diagram.svg");
  crate.addEntity("diagram.svg", diagram);

  Entity readme({"File"});
  readme.set("name", "README.md");
  crate.addEntity("README.md", readme);

  // Add hasPart to root data entity
  rootData.set("hasPart", mainEntity);
  rootData.set("hasPart", diagram);
  rootData.set("hasPart", readme);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/ro-crate-with-data-entity-as-mainentity.json",
    outputPath
  );
}

// REF: https://www.researchobject.org/ro-crate/specification/1.3/crate-focus.html (RO-Crates with a contextual entity as mainEntity)
// FIXTURE: tests/fixtures/ro-crate-with-contextual-entity-as-mainentity.json
TEST_CASE("RO-Crate with a contextual entity as mainEntity", "[integration]")
{
  ROCrate crate;

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("name", "Reibey, Mary (1777 - 1855)");
  rootData.set("license", "CC-BY");
  rootData.set("datePublished", "2023-01-01");

  // Create the mainEntity contextual entity
  Entity mainEntity({"Person"});
  mainEntity.set("name", "Mary Reibey");
  mainEntity.set("description", "Mary Reibey née Haydock (12 May 1777 – 30 May 1855) was an English-born merchant, shipowner and trader ...");
  crate.addEntity("https://en.wikipedia.org/wiki/Mary_Reibey", mainEntity);

  // Set mainEntity for root data entity
  rootData.set("mainEntity", mainEntity);

  // Create additional file entities
  Entity photo1({"File"});
  photo1.set("name", "photo1.jpg");
  crate.addEntity("photo1.jpg", photo1);

  Entity photo2({"File"});
  photo2.set("name", "photo2.jpg");
  crate.addEntity("photo2.jpg", photo2);

  // Add hasPart to root data entity
  rootData.set("hasPart", photo1);
  rootData.set("hasPart", photo2);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/ro-crate-with-contextual-entity-as-mainentity.json",
    outputPath
  );

}

