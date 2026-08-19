#pragma once

#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
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

  using Properties = std::unordered_map<Property, std::vector<PropertyValue>>;

  class Entity {
    
  public:
    Entity() = delete;
    explicit Entity(std::vector<std::string> types);
    ~Entity();
    
    void set(
      Property property,
      Value value,
      ValueType type = ValueType::Literal
    );
    void set(Property property, const Entity& entity);

  private:
    friend class ROCrate;

    void assignId(const std::string& id);
    std::shared_ptr<Properties> properties_;
  };

  inline Entity::Entity(std::vector<std::string> types) {
    /**
     * @brief Constructs an Entity with the specified types.
     *
     * @param types A vector of strings representing the types of the entity.
     * @throws std::invalid_argument if the types vector is empty.
     */
    
    // Initialise the properties map
    this->properties_ = std::make_shared<Properties>();
    
    // Validate types (reject empty)
    if ( types.empty() ) {
      throw std::invalid_argument("Entity must have at least one type.");
    }
    
    // Set the types of the entity
    std::vector<PropertyValue> typeValues;
    for (const auto& type : types)
      typeValues.push_back({type, ValueType::Literal});
    this->properties_->emplace("@type", typeValues);
  }
  
  inline Entity::~Entity() = default;

  inline void Entity::set(Property property, Value value, ValueType valueType) {
    /**
     * @brief Sets a property-value pair for the entity.
     *
     * @param property The name of the property to set.
     * @param value The value to assign to the property.
     * @param valueType The type of the value (Literal or Reference).
     * @throws std::invalid_argument if the property name or value is empty, or if attempting to set '@id' directly.
     */

    // Validate property and value
    if (property.empty()) 
      throw std::invalid_argument("Property name cannot be empty.");
    if (value.empty()) 
      throw std::invalid_argument("Property value cannot be empty.");
    if (property == "@id")
      throw std::invalid_argument("Cannot set '@id' property directly. "
                                  "Use ROCrate::addEntity to assign an ID.");

    // Add the value to the property in the properties map
    (*this->properties_)[property].push_back({value, valueType});
  }

  inline void Entity::set(Property property, const Entity& entity) {
    /**
     * @brief Sets a property to reference another entity.
     *
     * @param property The name of the property to set.
     * @param entity The entity to reference.
     * @throws std::invalid_argument if the property name is empty.
     * @throws std::runtime_error if the referenced entity does not have an '@id' property set.
     */
    
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
    /**
     * @brief Assigns an '@id' to the entity.
     *
     * @param id The identifier to assign to the entity.
     * @throws std::invalid_argument if the id is empty.
     */

    // Validate id
    if (id.empty()) {
      throw std::invalid_argument("Entity ID cannot be empty.");
    }

    // Assign the '@id' property to the entity's properties map
    (*this->properties_)["@id"] = {{id, ValueType::Literal}};
  }
  
  // ---------------------------------------------------------------------------
  // RO-Crate
  // ---------------------------------------------------------------------------

  using EntityRegister = std::unordered_map<std::string, Entity>;

  class ROCrate {
  public:
    ROCrate();
    
    void addEntity(const std::string& id, Entity& entity);
    Entity& getEntity(const std::string& id);
    void writeOut(const std::string& path);

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

    EntityRegister entities_;
  };

  inline ROCrate::ROCrate() {
    /**
     * @brief Constructs an RO-Crate with a root metadata entity and a root dataset entity.
     *
     * The constructor initializes the RO-Crate with an empty entity register, creates the root metadata entity
     * (ro-crate-metadata.json) and the root dataset entity, and adds the root dataset entity to the root metadata entity.
     */
    
    // Initialise the RO-Crate with an empty entity register
    this->entities_ = {};

    // Create the root metadata entity (ro-crate-metadata.json) 
    Entity rootEntity({"CreativeWork"});
    rootEntity.set(
        "conformsTo",
        "https://w3id.org/ro/crate/1.1",
        ValueType::Reference
    );
    this->addEntity("ro-crate-metadata.json", rootEntity);

    // Create the root dataset entity
    Entity datasetEntity({"Dataset"});
    this->addEntity("./", datasetEntity);

    // Add the root dataset entity to the root metadata entity
    rootEntity.set("about", datasetEntity);
  }

  inline void ROCrate::addEntity(const std::string& id, Entity& entity) {
    /**
     * @brief Adds an entity to the RO-Crate's entity register with the specified id.
     *
     * @param id The identifier for the entity.
     * @param entity The entity to add to the RO-Crate.
     * @throws std::invalid_argument if the id is empty.
     * @throws std::runtime_error if an entity with the given id already exists in the RO-Crate.
     */

    // Validate the id (reject empty)
    if (id.empty()) {
      throw std::invalid_argument("Entity ID cannot be empty.");
    }

    // Check if the entity with the given id already exists in the register
    if (this->entities_.find(id) != this->entities_.end()) {
      throw std::runtime_error("Entity with id '" + id + "' already exists in the RO-Crate.");
    }
    
    // Add the assigned ID to the Entity itself
    entity.assignId(id);

    // Add an entity to the RO-Crate's entity register
    this->entities_.emplace(id, entity);
  }

  inline Entity& ROCrate::getEntity(const std::string& id) {
    /**
     * @brief Retrieves an entity from the RO-Crate's entity register by its id.
     *
     * @param id The identifier of the entity to retrieve.
     * @return A reference to the entity with the specified id.
     * @throws std::runtime_error if no entity with the given id is found in the RO-Crate.
     */
    
    auto it = this->entities_.find(id);
    if (it == this->entities_.end()) {
      throw std::runtime_error("Entity with id '" + id + "' not found in the RO-Crate.");
    }
    return it->second;
  }

  inline void ROCrate::writeOut(const std::string& path) {
    /**
     * @brief Serializes the RO-Crate to a JSON file at the specified path.
     *
     * @param path The file path where the RO-Crate JSON will be written.
     * @throws std::runtime_error if there is an error writing to the file.
     */
    
    nlohmann::json outCrate = {
        {"@context", "https://w3id.org/ro/crate/1.1/context"},
        {"@graph", nlohmann::json::array()}
    };

    // Iterate over the entities, conver to json and append to the graph
    for (const auto& [id, entity] : this->entities_) {
      outCrate["@graph"].push_back(serializeEntity(id, entity));
    }

    // Write the JSON representation of the RO-Crate to a file
    std::ofstream outFile(path);
    if (!outFile) 
      throw std::runtime_error("Failed to open file for writing: " + path);

    outFile << outCrate.dump(4); // Pretty print with 4 spaces indentation
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
