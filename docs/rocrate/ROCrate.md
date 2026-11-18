---
generator: doxide
---


# ROCrate

**class ROCrate**

Represents an RO-Crate, a structured collection of entities and metadata.

The ROCrate class manages a collection of entities, including a root metadata entity and a root dataset entity.
It provides methods to add entities, retrieve entities by their identifiers, and manage the relationships between entities.


## Functions

| Name | Description |
| ---- | ----------- |
| [ROCrate](#ROCrate) | Constructs an RO-Crate with a root metadata entity and a root dataset entity. |
| [addEntity](#addEntity) | Adds an entity to the RO-Crate's entity register with the specified id. |
| [getEntity](#getEntity) | Retrieves an entity from the RO-Crate's entity register by its id. |
| [addContext](#addContext) | Adds a context entry to the RO-Crate's root metadata entity. |
| [writeOut](#writeOut) | Serializes the RO-Crate to a JSON file at the specified path. |
| [readIn](#readIn) | Reads an RO-Crate from a JSON file at the specified path and populates the current RO-Crate instance with the entities and context from the file. |

## Function Details

### ROCrate<a name="ROCrate"></a>
!!! function "ROCrate()"

    Constructs an RO-Crate with a root metadata entity and a root dataset entity.
    
    The constructor initializes the RO-Crate with an empty entity register, creates the root metadata entity
    (ro-crate-metadata.json) and the root dataset entity, and adds the root dataset entity to the root metadata entity.
    

### addContext<a name="addContext"></a>
!!! function "void addContext(const std::string&amp; key, const std::string&amp; value)"

    Adds a context entry to the RO-Crate's root metadata entity.
    
    
    :material-location-enter: `key`
    :    The context key to add.
        
    :material-location-enter: `value`
    :    The context value associated with the key.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the key or value is empty.
    

### addEntity<a name="addEntity"></a>
!!! function "void addEntity(const std::string&amp; id, Entity&amp; entity)"

    Adds an entity to the RO-Crate's entity register with the specified id.
    
    
    :material-location-enter: `id`
    :    The identifier for the entity.
        
    :material-location-enter: `entity`
    :    The entity to add to the RO-Crate.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the id is empty.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if an entity with the given id already exists in the RO-Crate.
    

### getEntity<a name="getEntity"></a>
!!! function "Entity&amp; getEntity(const std::string&amp; id)"

    Retrieves an entity from the RO-Crate's entity register by its id.
    
    
    :material-location-enter: `id`
    :    The identifier of the entity to retrieve.
        
    :material-keyboard-return: **Return**
    :    A reference to the entity with the specified id.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if no entity with the given id is found in the RO-Crate.
    

### readIn<a name="readIn"></a>
!!! function "void readIn(const std::string&amp; path)"

    Reads an RO-Crate from a JSON file at the specified path and populates
        the current RO-Crate instance with the entities and context from the file.
    
    
    :material-location-enter: `path`
    :    The file path from which to read the RO-Crate JSON.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if there is an error reading from the file or parsing the JSON.
    

### writeOut<a name="writeOut"></a>
!!! function "void writeOut(const std::string&amp; path)"

    Serializes the RO-Crate to a JSON file at the specified path.
    
    
    :material-location-enter: `path`
    :    The file path where the RO-Crate JSON will be written.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if there is an error writing to the file.
    

