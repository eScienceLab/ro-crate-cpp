#pragma once

#include <nlohmann/json.hpp>

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rocrate {

  using json = nlohmann::json;
  using Properties = std::vector<std::pair<std::string, json>>;

  class ROCrate;
  class Entity;

  // ---------------------------------------------------------------------------
  // Entity -- one node of the crate graph. Owns its data; JSON-LD is rendered
  // only when the crate is written out.
  // ---------------------------------------------------------------------------

  class Entity {
  public:
    // Replace the value of `property` (upsert).
    Entity& set(std::string property, json value) {
      props_[std::move(property)] = std::move(value);
      return *this;
    }

    // Append a reference {"@id": ref.id} to `property`, promoting scalars to
    // arrays.
    Entity& add(std::string property, const Entity& ref) {
      auto& slot = props_[std::move(property)];
      if (slot.is_null())        slot = json::array();
      else if (!slot.is_array()) slot = json::array({std::move(slot)});
      slot.push_back({{"@id", ref.id_}});
      return *this;
    }

    [[nodiscard]] const std::string& id() const noexcept { return id_; }

  private:
    friend class ROCrate;

    Entity(std::string id, std::vector<std::string> types)
      : id_(std::move(id)), types_(std::move(types)) {}

    std::string id_;
    std::vector<std::string> types_;
    std::map<std::string, json> props_;
  };

  // ---------------------------------------------------------------------------
  // ROCrate -- owns the graph of entities; JSON-LD produced only on demand.
  // ---------------------------------------------------------------------------

  class ROCrate {
  public:
    ROCrate()
      : context_("https://w3id.org/ro/crate/1.1/context") {
      auto& md = add("ro-crate-metadata.json", {"CreativeWork"});
      md.set("conformsTo", {{"@id", "https://w3id.org/ro/crate/1.1"}})
        .set("about", {{"@id", "./"}});
      add("./", {"Dataset"});
    }

    // Find-or-create the entity with @id; set @type and merge properties
    // (overwriting existing keys).
    Entity& add(std::string id, std::vector<std::string> types,
                Properties props = {}) {
      auto& e = entity(id);
      e.types_ = std::move(types);
      for (auto& [key, value] : props)
        e.props_[std::move(key)] = std::move(value);
      return e;
    }

    // Find-or-create by @id (creates a bare entity if unknown).
    Entity& entity(const std::string& id) {
      auto it = entities_.find(id);
      if (it == entities_.end())
        it = entities_.emplace(id, Entity(id, {})).first;
      return it->second;
    }

    // Render the whole crate as an RO-Crate JSON-LD document.
    [[nodiscard]] json to_json() const {
      json graph = json::array();
      for (const auto& [id, e] : entities_) {
        json node = json::object();
        node["@id"] = e.id_;
        node["@type"] = (e.types_.size() == 1) ? json(e.types_.front())
                                               : json(e.types_);
        for (const auto& [key, value] : e.props_)
          node[key] = value;
        graph.push_back(std::move(node));
      }
      return json{{"@context", context_}, {"@graph", std::move(graph)}};
    }

    void write(const std::string& path, int indent = 2) const {
      std::ofstream out(path);
      if (!out)
        throw std::runtime_error("ro-crate: cannot open '" + path
                                 + "' for writing");
      out << to_json().dump(indent) << '\n';
    }

  private:
    std::string context_;
    std::map<std::string, Entity> entities_;
  };

} // namespace rocrate
