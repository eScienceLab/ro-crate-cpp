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

To create an entity, instantiate the `Entity` class. You can must provide a type, 
or list of types.

```cpp
// Single type
Entity alice({"Person"});

// Multiple types
Entity data1({"File", "Dataset"});
```

### Add properties to an Entity

To add properties to an entity, use the `set` method. The first argument is the 
property name, and the second argument is the property value. The property value 
can either be a string, or another entity.

If a string property value is provided, it create a straightforward key-value pair 
in the entity e.g. `name: "Alice"`. If an entity is provided, it will create a 
relationship between the two entities e.g. `author: { "@id": "#alice" }`.

```cpp
// Add a string property
Entity alice({"Person"});
alice.set("name", "Alice");

// Add a relationship property
Entity data1({"File"});
data1.set("author", alice);
```

### Add an Entity to an RO-Crate

To add an entity to an RO-Crate, use the `addEntity` method. The first argument is
the entity ID, and the second argument is the entity object.

```cpp
ROCrate crate;

Entity alice({"Person"});
crate.addEntity("#alice", alice);
```

### Write out an RO-Crate

To write out an RO-Crate, use the `writeOut` method. The argument is the path to 
write the RO-Crate to. This will write out the `ro-crate-metadata.json` file.

```cpp
crate.writeOut("./routput/");o
```

## Explaination

These notes are mostly aimed at developers who want to understand the design of 
this library, but may be of interest to users as well.

### Philosophy of approach 

This header tries to avoid JSON manipulation as much as possible. It focuses
on a bare bones approach to RO-Crates; modelling an RO-Crate a flat list of entities, 
where each entity is a flat list of properties. The library does not attempt to 
model the RO-Crate specification in a more complex way, such as modelling the 
relationships between entities.

### Entity lifecycle

When an entity is created, it is not automatically added to the RO-Crate. The user 
must explicitly add the entity to the RO-Crate using the `addEntity` method.

When an entity is added to the RO-Crate, it is stored in a map of entities.

For maximum flexiblity, an already added entity may still be updated on the 
original entity object. The RO-Crate shares the same entity object, so any changes 
made to the original entity will be reflected in the RO-Crate.

Worked example:

```cpp
ROCrate crate;

Entity alice({"Person"});
alice.set("name", "Alice");

crate.addEntity("#alice", alice);

/* Crate state: 
{
  "#alice": {
    "type": ["Person"],
    "name": "Alice"
  }
}
*/

// Add an additional property to the crate entity
Entity crateAlice = crate.getEntity("#alice");
crateAlice.set("description", "One of hopefully many Contextual Entities");

// Add an additional property to the original entity object
alice.set("occupation", "Software Engineer");

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

## Reference

Reference documentation generated by [doxide](doxide.org) is available [here](./docs/index.md).
