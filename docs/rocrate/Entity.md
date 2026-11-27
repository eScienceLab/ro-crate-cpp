---
generator: doxide
---


# Entity

**class Entity**

Represents an entity in the RO-Crate.
An entity can have multiple types and properties, where each property can have multiple values.
Properties can be set as either literal values or references to other entities.

Entities are created and added via ROCrate::createEntity,
which constructs the Entity and returns a mutable reference to it.


## Functions

| Name | Description |
| ---- | ----------- |
| [set](#set) | Sets a property-value pair for the entity. |
| [set](#set) | Sets a property to reference another entity. |
| [set](#set) | Sets multiple properties for the entity at once. |
| [getProperties](#getProperties) | Retrieves the properties of the entity. |
| [Entity](#Entity) | Constructs an Entity with the specified types. |
| [assignId](#assignId) | Assigns an '@id' to the entity. |
| [validateSetRequest](#validateSetRequest) | Validates a set request for a property-value pair. |

## Function Details

### Entity<a name="Entity"></a>
!!! function "explicit Entity(std::vector&lt;std::string&gt; types)"

    Constructs an Entity with the specified types.
    
    Private since entities are always constructed and owned by an RO-Crate
    via ROCrate::createEntity.
    
    
    :material-location-enter: `types`
    :    A vector of strings representing the types of the entity.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the types vector is empty.
    

### assignId<a name="assignId"></a>
!!! function "void assignId(const std::string&amp; id)"

    Assigns an '@id' to the entity.
    
    
    :material-location-enter: `id`
    :    The identifier to assign to the entity.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the id is empty.
    

### getProperties<a name="getProperties"></a>
!!! function "Properties getProperties() const"

    Retrieves the properties of the entity.
    
    
    :material-keyboard-return: **Return**
    :    A copy of the properties map, where each property maps to a vector of PropertyValue objects.
    

### set<a name="set"></a>
!!! function "void set(Property property, Value value, ValueType type = ValueType::Literal)"

    Sets a property-value pair for the entity.
    
    
    :material-location-enter: `property`
    :    The name of the property to set.
        
    :material-location-enter: `value`
    :    The value to assign to the property.
        
    :material-location-enter: `valueType`
    :    The type of the value (Literal or Reference).
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the property name or value is empty, or if attempting to set '@id' directly.
    

!!! function "void set(Property property, const Entity&amp; entity)"

    Sets a property to reference another entity.
    
    
    :material-location-enter: `property`
    :    The name of the property to set.
        
    :material-location-enter: `entity`
    :    The entity to reference.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the property name is empty.
        
    :material-alert-circle-outline: **Throw**
    :    std::runtime_error if the referenced entity does not have an '@id' property set.
    

!!! function "void set(std::map&lt;Property, Value&gt; properties)"

    Sets multiple properties for the entity at once.
    
    
    :material-location-enter: `properties`
    :    A map of property names to their corresponding values.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if any property name or value is empty, or if attempting to set '@id' directly.
    

### validateSetRequest<a name="validateSetRequest"></a>
!!! function "void validateSetRequest(Property property, Value value)"

    Validates a set request for a property-value pair.
    
    
    :material-location-enter: `property`
    :    The name of the property to validate.
        
    :material-location-enter: `value`
    :    The value to validate.
        
    :material-alert-circle-outline: **Throw**
    :    std::invalid_argument if the property name or value is empty, or if attempting to set '@id' directly.
    

