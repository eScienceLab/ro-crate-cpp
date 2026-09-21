#pragma once

#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

namespace rocrate {

  // ---------------------------------------------------------------------------
  // Entity
  // ---------------------------------------------------------------------------

  using Property = std::string;
  using Value = std::string;
  enum class ValueType { Literal, Reference };

  struct PropertyValue {
    Value value;
    ValueType type;
  };

  using Properties = std::map<Property, std::vector<PropertyValue>>;

  /**
   * Represents an entity in the RO-Crate.
   * An entity can have multiple types and properties, where each property can have multiple values.
   * Properties can be set as either literal values or references to other entities.
   */
  class Entity {
    
  public:
    Entity() = delete;

    /**
     * Constructs an Entity with the specified types.
     *
     * @param types A vector of strings representing the types of the entity.
     * @throw std::invalid_argument if the types vector is empty.
     */
    explicit Entity(std::vector<std::string> types);
    ~Entity() = default;

    /**
     * Sets a property-value pair for the entity.
     *
     * @param property The name of the property to set.
     * @param value The value to assign to the property.
     * @param valueType The type of the value (Literal or Reference).
     * @throw std::invalid_argument if the property name or value is empty, or if attempting to set '@id' directly.
     */    
    void set(
      Property property,
      Value value,
      ValueType type = ValueType::Literal
    );
    
    /**
     * Sets a property to reference another entity.
     *
     * @param property The name of the property to set.
     * @param entity The entity to reference.
     * @throw std::invalid_argument if the property name is empty.
     * @throw std::runtime_error if the referenced entity does not have an '@id' property set.
     */
    void set(Property property, const Entity& entity);

  private:
    friend class ROCrate;

    /**
     * Assigns an '@id' to the entity.
     *
     * @param id The identifier to assign to the entity.
     * @throw std::invalid_argument if the id is empty.
     */
    void assignId(const std::string& id);
    std::shared_ptr<Properties> properties_;
  };

  inline Entity::Entity(std::vector<std::string> types)
    : properties_(std::make_shared<Properties>()) {
    
    // Validate types (reject empty)
    if ( types.empty() ) {
      throw std::invalid_argument("Entity must have at least one type.");
    }
    
    // Set the types of the entity
    std::vector<PropertyValue> typeValues;
    for (const auto& type : types)
      typeValues.push_back({type, ValueType::Literal});
    properties_->emplace("@type", typeValues);
  }
  
  inline void Entity::set(Property property, Value value, ValueType valueType) {
    // Validate property and value
    if (property.empty()) 
      throw std::invalid_argument("Property name cannot be empty.");
    if (value.empty()) 
      throw std::invalid_argument("Property value cannot be empty.");
    if (property == "@id")
      throw std::invalid_argument("Cannot set '@id' property directly. "
                                  "Use ROCrate::addEntity to assign an ID.");

    // Check property is not duplicate - ignore if so
    auto it = properties_->find(property);
    if (it != properties_->end()) {
      for (const auto& existingValue : it->second) {
        if (existingValue.value == value && existingValue.type == valueType) {
          // Duplicate found, ignore
          return;
        }
      }
    }

    // Add the value to the property in the properties map
    (*properties_)[property].push_back({value, valueType});
  }

  inline void Entity::set(Property property, const Entity& entity) {
    // Validate property name
    if (property.empty()) {
      throw std::invalid_argument("Property name cannot be empty.");
    }

    // Check if the entity has an '@id' property set
    const auto id = entity.properties_->find("@id");
    if (id == entity.properties_->end() || id->second.empty())
      throw std::runtime_error("Entity does not have an '@id' property set.");

    // Add JSON reference to the entity's '@id' to the property in the properties map
    set(property, id->second.front().value, ValueType::Reference);
  }

  inline void Entity::assignId(const std::string& id) {
    // Validate id
    if (id.empty()) {
      throw std::invalid_argument("Entity ID cannot be empty.");
    }

    // Assign the '@id' property to the entity's properties map
    (*properties_)["@id"] = {{id, ValueType::Literal}};
  }
  
  // ---------------------------------------------------------------------------
  // RO-Crate
  // ---------------------------------------------------------------------------

  using EntityRegister = std::map<std::string, Entity>;

  /**
   * Represents an RO-Crate, a structured collection of entities and metadata.
   *
   * The ROCrate class manages a collection of entities, including a root metadata entity and a root dataset entity.
   * It provides methods to add entities, retrieve entities by their identifiers, and manage the relationships between entities.
   */
  class ROCrate {
  public:
    /**
     * Constructs an RO-Crate with a root metadata entity and a root dataset entity.
     *
     * The constructor initializes the RO-Crate with an empty entity register, creates the root metadata entity
     * (ro-crate-metadata.json) and the root dataset entity, and adds the root dataset entity to the root metadata entity.
     */
    ROCrate();
    
    /**
     * Adds an entity to the RO-Crate's entity register with the specified id.
     *
     * @param id The identifier for the entity.
     * @param entity The entity to add to the RO-Crate.
     * @throw std::invalid_argument if the id is empty.
     * @throw std::runtime_error if an entity with the given id already exists in the RO-Crate.
     */
    void addEntity(const std::string& id, Entity& entity);

    /**
     * Retrieves an entity from the RO-Crate's entity register by its id.
     *
     * @param id The identifier of the entity to retrieve.
     * @return A reference to the entity with the specified id.
     * @throw std::runtime_error if no entity with the given id is found in the RO-Crate.
     */
    Entity& getEntity(const std::string& id);

    /**
     * Adds a context entry to the RO-Crate's root metadata entity.
     *
     * @param key The context key to add.
     * @param value The context value associated with the key.
     * @throw std::invalid_argument if the key or value is empty.
     */
    void addContext(const std::string& key, const std::string& value);

    /**
     * Serializes the RO-Crate to a JSON file at the specified path.
     *
     * @param path The file path where the RO-Crate JSON will be written.
     * @throw std::runtime_error if there is an error writing to the file.
     */
    void writeOut(const std::string& path);

    /**
      * Reads an RO-Crate from a JSON file at the specified path and populates
      * the current RO-Crate instance with the entities and context from the file.
      *
      * @param path The file path from which to read the RO-Crate JSON.
      * @throw std::runtime_error if there is an error reading from the file or parsing the JSON.
     */
    void readIn(const std::string& path);

  private:
    nlohmann::json serializeEntity(
        const std::string& id,
        const Entity& entity
    ) const;

    nlohmann::json serializePropertyValues(
        const std::vector<PropertyValue>& values
    ) const;

    nlohmann::json serializePropertyValue(
        const PropertyValue& value
    ) const;

    void updateProperties(
      Entity& entity,
      const nlohmann::json& jsonEntity
    );

    EntityRegister entities_;
    std::map<std::string, std::string> context_;
  };

  inline ROCrate::ROCrate() {
    // Initialise the RO-Crate with an empty entity register
    entities_ = {};

    // Create the root metadata entity (ro-crate-metadata.json) 
    Entity rootEntity({"CreativeWork"});
    rootEntity.set(
        "conformsTo",
        "https://w3id.org/ro/crate/1.3",
        ValueType::Reference
    );
    addEntity("ro-crate-metadata.json", rootEntity);

    // Create the root dataset entity
    Entity datasetEntity({"Dataset"});
    addEntity("./", datasetEntity);

    // Add the root dataset entity to the root metadata entity
    rootEntity.set("about", datasetEntity);
  }

  inline void ROCrate::addEntity(const std::string& id, Entity& entity) {
    // Validate the id (reject empty)
    if (id.empty()) {
      throw std::invalid_argument("Entity ID cannot be empty.");
    }

    // Check if the entity with the given id already exists in the register
    if (entities_.find(id) != entities_.end()) {
      throw std::runtime_error("Entity with id '" + id + "' already exists in the RO-Crate.");
    }
    
    // Add the assigned ID to the Entity itself
    entity.assignId(id);

    // Add an entity to the RO-Crate's entity register
    entities_.emplace(id, entity);
  }

  inline Entity& ROCrate::getEntity(const std::string& id) {
    auto it = entities_.find(id);
    if (it == entities_.end()) {
      throw std::runtime_error("Entity with id '" + id + "' not found in the RO-Crate.");
    }
    return it->second;
  }

  inline void ROCrate::addContext(const std::string& key, const std::string& value) {
    // Validate the context
    if (key.empty()) {
      throw std::invalid_argument("Context key cannot be empty.");
    }
    if (value.empty()) {
      throw std::invalid_argument("Context value cannot be empty.");
    }

    // Add the context entry to the RO-Crate's root metadata entity
    context_[key] = value;
  }

  inline void ROCrate::writeOut(const std::string& path) {
    // Create the JSON representation of the RO-Crate
    nlohmann::json outCrate = {
        {"@context", "https://w3id.org/ro/crate/1.3/context"},
        {"@graph", nlohmann::json::array()}
    };

    // Add the context entries to the JSON representation
    if (!context_.empty()) {
      outCrate["@context"] = nlohmann::json::array();
      outCrate["@context"].push_back("https://w3id.org/ro/crate/1.3/context");
      for (const auto& [key, value] : context_) {
        outCrate["@context"].push_back({{key, value}});
      }
    }

    // Iterate over the entities, conver to json and append to the graph
    for (const auto& [id, entity] : entities_) {
      outCrate["@graph"].push_back(serializeEntity(id, entity));
    }

    // Write the JSON representation of the RO-Crate to a file
    std::ofstream outFile(path);
    if (!outFile) 
      throw std::runtime_error("Failed to open file for writing: " + path);

    outFile << outCrate.dump(4); // Pretty print with 4 spaces indentation
  }

  inline void ROCrate::readIn(const std::string& path) {
    // Read the JSON representation of the RO-Crate from a file
    std::ifstream inFile(path);
    if (!inFile) 
      throw std::runtime_error("Failed to open file for reading: " + path);

    // Check if valid JSON
    nlohmann::json inCrate;
    try {
      inFile >> inCrate;
    } catch (const nlohmann::json::parse_error& e) {
      throw std::runtime_error("Failed to parse JSON from file: " + path + ". Error: " + e.what());
    }

    // Validate the RO-Crate structure
    if (!inCrate.contains("@context") || !inCrate.contains("@graph")) {
      throw std::runtime_error("Invalid RO-Crate structure in file: " + path);
    }

    // Upsert the context entries from the JSON representation into the RO-Crate
    if (inCrate["@context"].is_array()) {
      for (const auto& contextEntry : inCrate["@context"]) {
        if (contextEntry.is_object()) {
          for (const auto& [key, value] : contextEntry.items()) {
            addContext(key, value.get<std::string>());
          }
        }
      }
    }

    // Iterate over the entities in the JSON representation and add them to the RO-Crate
    for (const auto& entityJson : inCrate["@graph"]) {
      // Validate that the entity has an '@id' field
      if (!entityJson.contains("@id")) {
        throw std::runtime_error("Entity in RO-Crate JSON is missing '@id' field.");
      }
      std::string entityId = entityJson["@id"].get<std::string>();

      // Get the entity type
      if (!entityJson.contains("@type")) {
        throw std::runtime_error("Entity in RO-Crate JSON is missing '@type' field.");
      }
      std::string entityType = entityJson["@type"].get<std::string>();

      // Try getting the entity from the register, if it exists, otherwise create a new one
      Entity entity({"Thing"}); // Default type, will be overridden if entity exists
      try {
        // Existing entity
        entity = getEntity(entityId);
        updateProperties(entity, entityJson);
      } catch (const std::runtime_error&) {
        // Entity does not exist, create a new one
        entity = Entity({entityType});
        updateProperties(entity, entityJson);
        addEntity(entityId, entity);
      }
    }
  }

  inline void ROCrate::updateProperties(Entity& entity, const nlohmann::json& entityJson) {
      // Iterate over the properties of the entity and add them to the Entity object
      for (const auto& [property, value] : entityJson.items()) {
        if (property == "@id") {
          continue; // Skip the @id property
        }

        // Handle the property value based on its type (array, object, or literal)
        if (value.is_array()) {
          // If array, iterate over the items and add them to the Entity object
          for (const auto& item : value) {
            if (item.is_object() && item.contains("@id")) {
              entity.set(property, item["@id"].get<std::string>(), ValueType::Reference);
            } else {
              entity.set(property, item.get<std::string>(), ValueType::Literal);
            }
          }
        } else if (value.is_object() && value.contains("@id")) {
          entity.set(property, value["@id"].get<std::string>(), ValueType::Reference);
        } else {
          entity.set(property, value.get<std::string>(), ValueType::Literal);
        }
      }
  }

  inline nlohmann::json ROCrate::serializePropertyValue(
      const PropertyValue& value
  ) const {
    if (value.type == ValueType::Reference) {
      return {{"@id", value.value}};
    }

    return value.value;
  }

  inline nlohmann::json ROCrate::serializePropertyValues(
      const std::vector<PropertyValue>& values
  ) const {
    nlohmann::json serialized = nlohmann::json::array();

    for (const auto& value : values) {
      serialized.push_back(serializePropertyValue(value));
    }

    if (serialized.size() == 1) {
      return serialized.front();
    }

    return serialized;
  }

  inline nlohmann::json ROCrate::serializeEntity(
      const std::string& id,
      const Entity& entity
  ) const {
    nlohmann::json serialized;
    serialized["@id"] = id;

    for (const auto& [property, values] : *entity.properties_) {
      if (property == "@id") {
        continue;
      }

      serialized[property] = serializePropertyValues(values);
    }

    return serialized;
  }
}
