# ro-crate-cpp

A header only library for creating RO-Crates in C++ 

## Tutorial

### Installation

To get started using this library, you should download `ro-crate.hpp` from the `include` directory and include it in your project. The library is designed to be largely self contained.

The `nlohmann/json` library is used for JSON serialization and deserialization, so make sure to include it in your project as well. It is available in the `vendor` directory of this repository.

### Usage

To demostrate usage of this library we will create one of the example RO-Crates documented on the [RO-Crate website](https://www.researchobject.org/ro-crate/examples). Specifically, ["Example with with file, author, location"](https://www.researchobject.org/ro-crate/specification/1.1/appendix/jsonld).

```cpp
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
    Entity& root = crate.getEntity("ro-crate-metadata.json");
    root.set("description", "RO-Crate Metadata File Descriptor (this file)");

    // Add name, description to the root data entity (./)
    Entity& rootData = crate.getEntity("./");
    rootData.set("name", "Example RO-Crate");
    rootData.set("description", "The RO-Crate Root Data Entity");

    // --------------------------------------------------------------------------
    // Create the person, which is a contextual entity, and add it to the crate
    
    Entity& alice = crate.addEntity("#alice", {"Person"});
    alice.set("name", "Alice");
    alice.set("description", "One of hopefully many Contextual Entities");

    // --------------------------------------------------------------------------
    // Create the place, another contextual entity, and add it to the crate
    Entity& catalinaPark = crate.addEntity("http://sws.geonames.org/8152662/", {"Place"});
    catalinaPark.set("name", "Catalina Park");

    // --------------------------------------------------------------------------
    // Create two datasets, which are data entities, and add them to the crate.
    Entity& data1 = crate.addEntity("data1.txt", {"File"});
    data1.set("description", "One of hopefully many Data Entities");
    data1.set("author", alice);
    data1.set("contentLocation", catalinaPark);
    rootData.set("hasPart", data1); // Ensure that the root data entity has a hasPart relationship to data1

    Entity& data2 = crate.addEntity("data2.txt", {"File"});
    rootData.set("hasPart", data2); 

    // --------------------------------------------------------------------------
    // Write out
    const std::string outputPath = std::string("ro-crate-metadata.json");
    crate.writeOut(outputPath);
}

int main() {
    create_example_ro_crate();
}
```

## How-To

### Create an RO-Crate

To create an RO-Crate, simply instantiate the `ROCrate` class. This will 
automatically create the root metadata entity (ro-crate-metadata.json) and the 
root data entity (./).

```cpp
#include "ro-crate.hpp"

ROCrate crate;
```

### Create an Entity

Entities are always owned by an RO-Crate: creating one and adding it to the
crate happen in a single step, via the `addEntity` method on `ROCrate`. The
first argument is the entity ID, and the second is a type, or list of types.
`addEntity` constructs the entity and returns a mutable reference to it.

```cpp
ROCrate crate;

// Single type
Entity& alice = crate.addEntity("#alice", {"Person"});

// Multiple types
Entity& data1 = crate.addEntity("data1.txt", {"File", "Dataset"});
```

### Add properties to an Entity

To add properties to an entity, use the `set` method on the reference returned
by `addEntity` (or `getEntity`). The first argument is the property name, and 
the second argument is the property value. The property value can either be a 
string, or another entity.

If a string property value is provided, it create a straightforward key-value pair 
in the entity e.g. `name: "Alice"`. If an entity is provided, it will create a 
relationship between the two entities e.g. `author: { "@id": "#alice" }`.

```cpp
ROCrate crate;

// Add a string property
Entity& alice = crate.addEntity("#alice", {"Person"});
alice.set("name", "Alice");

// Add a relationship property
Entity& data1 = crate.addEntity("data1.txt", {"File"});
data1.set("author", alice);
```

You can also bulk add properties to an entity by passing a map of property names 
and values to the `set` method.

```cpp
ROCrate crate;
Entity& alice = crate.addEntity("#alice", {"Person"});
std::map<std::string, std::string> properties = {
    {"name", "Alice"},
    {"description", "One of hopefully many Contextual Entities"}
};
```

### Add additional context to an RO-Crate

To add additional context to an RO-Crate, use the `addContext` method. The first 
argument is the id of the context to add. The second is the context link itself 
which must be non-empty.

```cpp
ROCrate crate;

crate.addContext("txc", "https://purl.archive.org/language-data-commons/terms#");
```

### Write out an RO-Crate

To write out an RO-Crate, use the `writeOut` method. The argument is the path to 
write the RO-Crate to. This will write out the `ro-crate-metadata.json` file.

```cpp
crate.writeOut("./routput/ro-crate-metadata.json");
```

## Explanation

These notes are mostly aimed at developers who want to understand the design of 
this library, but may be of interest to users as well.

### Philosophy of approach 

This header tries to avoid JSON manipulation as much as possible. It focuses
on a bare bones approach to RO-Crates; modelling an RO-Crate a flat list of entities, 
where each entity is a flat list of properties. The library does not attempt to 
model the RO-Crate specification in a more complex way, such as modelling the 
relationships between entities.

### Entity lifecycle

An entity cannot exist independently of an RO-Crate: it is always constructed 
and owned by the crate's entity register, via `ROCrate::addEntity`. This method 
returns a mutable reference to the entity, which can be used to set further 
properties, or passed to `set` on other entities to create relationships.

Worked example:

```cpp
ROCrate crate;

Entity& alice = crate.addEntity("#alice", {"Person"});
alice.set("name", "Alice");

/* Crate state: 
{
  "#alice": {
    "type": ["Person"],
    "name": "Alice"
  }
}
*/

// Further updates go through the same reference, or a fresh one from getEntity
alice.set("description", "One of hopefully many Contextual Entities");
crate.getEntity("#alice").set("occupation", "Software Engineer");

/* Crate state:
{
  "#alice": {
    "type": ["Person"],
    "name": "Alice",
    "description": "One of hopefully many Contextual Entities",
    "occupation": "Software Engineer"
  }
}
*/
```

Note that `getEntity` and `addEntity` both return references (`Entity&`). 
Copying the result into a plain `Entity` variable (rather than binding a 
reference) makes an independent copy that will not reflect further changes, 
and will not itself reflect changes made to the crate's entity afterwards.

## Reference

Reference documentation generated by [doxide](doxide.org) is available [here](http://esciencelab.org.uk/ro-crate-cpp/).
