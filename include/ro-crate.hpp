#pragma once

#include <nlohmann/json.hpp>

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>

namespace rocrate {

  // ---------------------------------------------------------------------------
  // Entity
  // ---------------------------------------------------------------------------

  using Property = std::string;
  using Value = std::string;
  using Properties = std::unordered_map<Property, std::vector<Value>>;

  class Entity {
    
  public:
    Entity() = delete;
    Entity(std::vector<std::string> types);
    ~Entity();
    
    void set(Property property, Value value);
    void set(Property property, const Entity& entity);

  private:
    friend class ROCrate;

    void assignId(const std::string& id);
    std::shared_ptr<Properties> properties_;
  };

  inline Entity::Entity(std::vector<std::string> types) {
    // Initialise the properties map
    this->properties_ = std::make_shared<Properties>();
    
    // Validate types (reject empty)
    if ( types.empty() ) {
      throw std::invalid_argument("Entity must have at least one type.");
    }
    
    // Set the type of the entity
    this->properties_->emplace("@type", types);
  }
  
  inline Entity::~Entity() {}

  inline void Entity::set(Property property, Value value) {
    // Add / update a property-value pair to the entity's properties
    if (property.empty()) {
      throw std::invalid_argument("Property name cannot be empty.");
    } else if (value.empty()) {
      throw std::invalid_argument("Property value cannot be empty.");
    } else if (property == "@id") {
      throw std::invalid_argument("Cannot set '@id' property directly. Use ROCrate::addEntity to assign an ID.");
    }

    // Add the value to the property in the properties map
    (*this->properties_)[property].push_back(value);
  }

  inline void Entity::set(Property property, const Entity& entity) {
    // Add / update a property-entity pair to the entity's properties
    if (property.empty()) {
      throw std::invalid_argument("Property name cannot be empty.");
    }

    // Check if the entity has an '@id' property set
    const auto id = entity.properties_->find("@id");
    if (id == entity.properties_->end() || id->second.empty())
      throw std::runtime_error("Entity does not have an '@id' property set.");

    // Add the '@id' of the linked entity to the current entity's properties
    (*this->properties_)[property].push_back(id->second.front());
  }

  inline void Entity::assignId(const std::string& id) {
    (*this->properties_)["@id"] = {id};
  }
  
  // ---------------------------------------------------------------------------
  // RO-Crate
  // ---------------------------------------------------------------------------

  using EntityRegister = std::unordered_map<std::string, Entity>;

  class ROCrate {
  public:
    ROCrate();
    
    void addEntity(const std::string& id, Entity& entity);
    Entity& getEntity(std::string id);
    void writeOut();

  private:
    EntityRegister entities_;
  };

  inline ROCrate::ROCrate() {
    // Initialise the RO-Crate with an empty entity register
    this->entities_ = {};

    // Create the root metadata entity (ro-crate-metadata.json) 
    Entity rootEntity({"CreativeWork"});
    rootEntity.set("conformsTo", "https://w3id.org/ro/crate/1.1");
    this->addEntity("ro-crate-metadata.json", rootEntity);

    // Create the root dataset entity
    Entity datasetEntity({"Dataset"});
    this->addEntity("./", datasetEntity);
  }

  inline void ROCrate::addEntity(const std::string& id, Entity& entity) {
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

  inline Entity& ROCrate::getEntity(std::string id) {
    // Retrieve an entity from the RO-Crate's entity register by its id
    auto it = this->entities_.find(id);
    if (it == this->entities_.end()) {
      throw std::runtime_error("Entity with id '" + id + "' not found in the RO-Crate.");
    }
    return it->second;
  }

  inline void ROCrate::writeOut() {
    // Serialise the RO-Crate to a JSON file
  }
}
