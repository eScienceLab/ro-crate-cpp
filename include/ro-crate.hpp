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
    
    // Set the type of the entity
    this->properties_->emplace("@type", types);
  }
  
  inline Entity::~Entity() {}

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
    (*this->properties_)["#ref-"+property].push_back(id->second.front());
  }

  inline void Entity::assignId(const std::string& id) {
    /**
     * @brief Assigns an '@id' to the entity.
     *
     * @param id The identifier to assign to the entity.
     * @throws std::invalid_argument if the id is empty.
     */
    
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
    void writeOut(const std::string& path);

  private:
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
    rootEntity.set("#ref-conformsTo", "https://w3id.org/ro/crate/1.1");
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

  inline Entity& ROCrate::getEntity(std::string id) {
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
    
    // Serialise the RO-Crate to a JSON file
    nlohmann::json outCrate;

    // Create context and graph entities
    outCrate["@context"] = "https://w3id.org/ro/crate/1.1/context";
    outCrate["@graph"] = nlohmann::json::array();

    // Iterate over the entities, conver to json and append to the graph
    for (const auto& [id, entity] : this->entities_) {
      nlohmann::json jsonEntity;

      // Add the '@id' property
      jsonEntity["@id"] = id;

      // Add other properties
      for (const auto& [property, values] : *(entity.properties_)) {
        // Skip the '@id' property since it's already added
        if (property == "@id") 
          continue;

        if (property.rfind("#ref-", 0) == 0) {
          // Handle reference properties 
          
          std::string refPropertyName = property.substr(5); // Remove the "#ref-" prefix
          if (values.size() == 1) {
            jsonEntity[refPropertyName] = {{"@id", values.front()}};
          } else {
            jsonEntity[refPropertyName] = nlohmann::json::array();
            for (const auto& value : values) {
              jsonEntity[refPropertyName].push_back({{"@id", value}});
            }
          }
        } else {
          // Handle regular properties
          
          if (values.size() == 1) {
            jsonEntity[property] = values.front();
          } else {
            jsonEntity[property] = values;
          }
        }
      }
      
      // Append the entity to the graph
      outCrate["@graph"].push_back(jsonEntity);
    }

    // Write the JSON representation of the RO-Crate to a file
    std::ofstream outFile(path);
    if (!outFile) {
      throw std::runtime_error("Failed to open file for writing: ro-crate-metadata.json");
    }

    outFile << outCrate.dump(4); // Pretty print with 4 spaces indentation

    outFile.close();
  }
}
