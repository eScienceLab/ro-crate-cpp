#include <catch2/catch_test_macros.hpp>

#include "test_helpers.hpp"
#include <ro-crate.hpp>

using rocrate::Entity;
using rocrate::ROCrate;
using rocrate::ValueType;

// REF: https://www.researchobject.org/ro-crate/specification/1.3/root-data-entity.html#minimal-example-of-ro-crate
// FIXTURE: tests/fixtures/minimal-example-of-ro-crate.json
TEST_CASE("Minimal RO-Crate", "[integration]")
{
  ROCrate crate;

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("cite-as", "https://doi.org/10.4225/59/59672c09f4a4b");
  rootData.set("datePublished", "2017");
  rootData.set("name", "Data files associated with the manuscript:Effects of facilitated family case conferencing for ...");
  rootData.set("description", "Palliative care planning for nursing home residents with advanced dementia ...");
  rootData.set("creditText", "Agar, M. et al., 2017. Data supporting \"Effects of facilitated family case conferencing for advanced dementia: A cluster randomised clinical trial\". https://doi.org/10.4225/59/59672c09f4a4b");

  // Create the license entity
  Entity license({"CreativeWork"});
  license.set("description", "This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Australia License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/au/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.");
  license.set("identifier", "https://creativecommons.org/licenses/by-nc-sa/3.0/au/");
  license.set("name", "Attribution-NonCommercial-ShareAlike 3.0 Australia (CC BY-NC-SA 3.0 AU)");

  crate.addEntity("https://creativecommons.org/licenses/by-nc-sa/3.0/au/", license);

  // Create the DOI entry
  Entity doi({"PropertyValue"});
  doi.set("propertyID", "https://registry.identifiers.org/registry/doi");
  doi.set("value", "doi:10.4225/59/59672c09f4a4b");
  doi.set("url", "https://doi.org/10.4225/59/59672c09f4a4b");
  crate.addEntity("https://doi.org/10.4225/59/59672c09f4a4b", doi);
  rootData.set("identifier", doi);

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
  rootData.set("author", author);

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

// REF: https://www.researchobject.org/ro-crate/specification/1.3/crate-focus.html (RO-Crates which focus on multiple Contextual Entities)
// FIXTURE: tests/fixtures/ro-crate-which-focus-on-multiple-contextual-entities.json
TEST_CASE("RO-Crate which focuses on multiple Contextual Entities", "[integration]")
{
  ROCrate crate;

  // Add metadata to the RO-Crate metadata file descriptor
  Entity metadataFileDescriptor = crate.getEntity("ro-crate-metadata.json");
  metadataFileDescriptor.set("description", "RO-Crate Metadata File Descriptor (this file)");

  // Add metadata to root data entity
  Entity rootData = crate.getEntity("./");
  rootData.set("name", "Language Data Ontology");
  rootData.set("description", "This is an experimental language data ontology based on OLAC terms for use in the ATAP and LDaCA projects");
  
  // Add additonal context
  crate.addContext("txc", "https://purl.archive.org/language-data-commons/terms#");

  // Create the first contextual entity
  Entity annotation({"rdfs:Class"});
  annotation.set("name", "Annotation");
  annotation.set("sameAs", "http://www.language-archives.org/REC/type-20020628.html#annotation");
  annotation.set("rdfs:comment", "The resource includes information which annotates some other linguistic record.");
  annotation.set("rdfs:label", "Annotation");
  annotation.set("rdfs:subClassOf", "schema:CreativeWork", ValueType::Reference);
  crate.addEntity("txc:Annotation", annotation);

  // Create the second contextual entity
  Entity collectionEvent({"rdfs:Class"});
  collectionEvent.set("name", "CollectionEvent");
  collectionEvent.set("rdfs:comment", "A description of an event at which one or more PrimaryTexts were captured, e.g. as video or audio");
  collectionEvent.set("rdfs:label", "CollectionEvent");
  collectionEvent.set("rdfs:subClassOf", "schema:Event", ValueType::Reference);
  collectionEvent.set("rdfs:subClassOf", "schema:CreateAction", ValueType::Reference);
  crate.addEntity("txc:CollectionEvent", collectionEvent);

  // Add mentions to root data entity
  rootData.set("mentions", annotation);
  rootData.set("mentions", collectionEvent);

  // Write out
  const std::string outputPath =
    std::string(TEST_SOURCE_DIR) + "/ro-crate-metadata.json";
  crate.writeOut(outputPath);

  REQUIRE_RO_CRATE_FILE_EQUAL_BY_ID(
    std::string(TEST_SOURCE_DIR) +
    "/tests/fixtures/ro-crate-which-focus-on-multiple-contextual-entities.json",
    outputPath
  );
}
