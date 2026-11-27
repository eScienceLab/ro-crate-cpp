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
| [createEntity](#createEntity) | Constructs an entity with the given types and adds it to the RO-Crate's entity register. |
| [hasEntity](#hasEntity) | Checks if an entity with the specified id exists in the RO-Crate's entity register. |
| [getEntity](#getEntity) | Retrieves an entity from the RO-Crate's entity register by its id. |
| [getEntities](#getEntities) | Gets the register of entities in the RO-Crate. |
| [addContext](#addContext) | Adds a context entry to the RO-Crate's root metadata entity, without a prefix. |
| [addContext](#addContext) | Adds a context entry to the RO-Crate's root metadata entity, with a prefix. |
| [getContext](#getContext) | Retrieves the context entries :material-keyboard-return: **Return** :    A copy of the context entries map  |
| [getRemoteContext](#getRemoteContext) | Retrieves the remote context entries :material-keyboard-return: **Return** :    A copy of the remote context entries vector  |
| [writeOut](#writeOut) | Serializes the RO-Crate to a JSON file at the specified path. |
| [readIn](#readIn) | Reads an RO-Crate from a JSON file at the specified path and populates the current RO-Crate instance with the entities and context from the file. |

## Function Details

### ROCrate<a name="ROCrate"></a>
!!! function "ROCrate(std::string rootDataEntityId = &quot;./&quot;)"

    Constructs an RO-Crate with a root metadata entity and a root dataset entity.
    
    The constructor initializes the RO-Crate with an empty entity register, creates the root metadata entity
    (ro-crate-metadata.json) and the root dataset entity, and adds the root dataset entity to the root metadata entity.
    
    If no rootDataEntityId is provided, a standard RO-Crate is produced, if
    a rootDataEntityId is provided, a detached RO-Crate is produced, assuming
    that rootDataEntityId is a valid URI
    
    
    :material-location-enter: `rootDataEntityId`
    :    The identifier for the root dataset entity. Defaults to "./" if not provided.
    

### addContext<a name="addContext"></a>
!!! function "void addContext(const std::string&amp; value)"

    Adds a context entry to the RO-Crate's root metadata entity, without a prefix.
    
    
    :material-location-enter: `value`
    :    The context value associated with the key.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the value is empty.
    

!!! function "void addContext(const std::string&amp; key, const std::string&amp; value)"

    Adds a context entry to the RO-Crate's root metadata entity, with a prefix.
    
    
    :material-location-enter: `key`
    :    The context key to add.
        
    :material-location-enter: `value`
    :    The context value associated with the key.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the key or value is empty.
    

### createEntity<a name="createEntity"></a>
!!! function "Entity&amp; createEntity(const std::string&amp; id, std::vector&lt;std::string&gt; types)"

    Constructs an entity with the given types and adds it to the RO-Crate's entity register.
    
    
    :material-location-enter: `id`
    :    The identifier for the entity.
        
    :material-location-enter: `types`
    :    A vector of strings representing the types of the entity.
        
    :material-keyboard-return: **Return**
    :    A mutable reference to the newly created entity, now owned by the RO-Crate.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the id is empty, or if the types vector is empty.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if an entity with the given id already exists in the RO-Crate.
    

### getContext<a name="getContext"></a>
!!! function "std::map&lt;std::string, std::string&gt; getContext() const"

    Retrieves the context entries
    
    
    :material-keyboard-return: **Return**
    :    A copy of the context entries map
    

### getEntities<a name="getEntities"></a>
!!! function "EntityRegister getEntities() const"

    Gets the register of entities in the RO-Crate.
    
    
    :material-keyboard-return: **Return**
    :    A copy of the entity register
    

### getEntity<a name="getEntity"></a>
!!! function "Entity&amp; getEntity(const std::string&amp; id)"

    Retrieves an entity from the RO-Crate's entity register by its id.
    
    
    :material-location-enter: `id`
    :    The identifier of the entity to retrieve.
        
    :material-keyboard-return: **Return**
    :    A reference to the entity with the specified id.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if no entity with the given id exists in the RO-Crate.
    

### getRemoteContext<a name="getRemoteContext"></a>
!!! function "std::vector&lt;std::string&gt; getRemoteContext() const"

    Retrieves the remote context entries
    
    
    :material-keyboard-return: **Return**
    :    A copy of the remote context entries vector
    

### hasEntity<a name="hasEntity"></a>
!!! function "bool hasEntity(const std::string&amp; id)"

    Checks if an entity with the specified id exists in the RO-Crate's entity register.
    
    
    :material-location-enter: `id`
    :    The identifier of the entity to check.
        
    :material-keyboard-return: **Return**
    :    true if an entity with the given id exists, false otherwise.
    

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
    

