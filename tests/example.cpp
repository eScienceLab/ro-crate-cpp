// --------------------------------------------------------------------------
// Include the ro-crate header file
#include "ro-crate.hpp"

using rocrate::Entity;
using rocrate::ROCrate;

void create_example_ro_crate() {

    // --------------------------------------------------------------------------
    // Create an RO-Crate object, which will automatically create the root 
    // metadata entity (ro-crate-metadata.json) and the root data entity (./)
    ROCrate crate;
 
    // --------------------------------------------------------------------------
    // Add description to the root metadata entity (ro-crate-metadata.json) as 
    // this is currently missing
    
    // Get the root metadata entity from the crate and set the description
    Entity root = crate.getEntity("ro-crate-metadata.json");
    root.set("description", "RO-Crate Metadata File Descriptor (this file)");

    // Add name, description to the root data entity (./)
    Entity rootData = crate.getEntity("./");
    rootData.set("name", "Example RO-Crate");
    rootData.set("description", "The RO-Crate Root Data Entity");

    // --------------------------------------------------------------------------
    // Create the person, which is a contextual entity, and add it to the crate
    
    Entity alice({"Person"});
    alice.set("name", "Alice");
    alice.set("description", "One of hopefully many Contextual Entities");
    crate.addEntity("#alice", alice);

    // --------------------------------------------------------------------------
    // Create the place, another contextual entity, and add it to the crate
    Entity catalinaPark({"Place"});
    catalinaPark.set("name", "Catalina Park");
    crate.addEntity("http://sws.geonames.org/8152662/", catalinaPark);

    // --------------------------------------------------------------------------
    // Create two datasets, which are data entities, and add them to the crate.
    Entity data1({"File"});
    data1.set("description", "One of hopefully many Data Entities");
    data1.set("author", alice);
    data1.set("contentLocation", catalinaPark);
    crate.addEntity("data1.txt", data1);
    rootData.set("hasPart", data1); // Ensure that the root data entity has a hasPart relationship to data1

    Entity data2({"File"});
    crate.addEntity("data2.txt", data2);
    rootData.set("hasPart", data2); 

    // --------------------------------------------------------------------------
    // Write out
    const std::string outputPath = std::string("ro-crate-metadata.json");
    crate.writeOut(outputPath);
}

int main() {
    create_example_ro_crate();
    return 0;
}
