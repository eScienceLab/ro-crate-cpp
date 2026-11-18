---
generator: doxide
---


# Entity

**class Entity**

Represents an entity in the RO-Crate.
An entity can have multiple types and properties, where each property can have multiple values.
Properties can be set as either literal values or references to other entities.


## Functions

| Name | Description |
| ---- | ----------- |
| [Entity](#Entity) | Constructs an Entity with the specified types. |
| [set](#set) | Sets a property-value pair for the entity. |
| [set](#set) | Sets a property to reference another entity. |
| [assignId](#assignId) | Assigns an '@id' to the entity. |

## Function Details

### Entity<a name="Entity"></a>
!!! function "explicit Entity(std::vector&lt;std::string&gt; types)"

    Constructs an Entity with the specified types.
    
    
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
    

### set<a name="set"></a>
!!! function "void set( Property property, Value value, ValueType type = ValueType::Literal )"

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
    

