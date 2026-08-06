#pragma once

#include <nlohmann/json.hpp>

class ROCrate
{
public:
  ROCrate()
    : crate_(nlohmann::json::object())
  {
    // Add the context entry
    crate_["@context"] = nlohmann::json::array(
        { "https://w3id.org/ro/crate/1.1/context" }
      );

    // Add the metadata file entry
    crate_["@graph"] = nlohmann::json::array();
    crate_["@graph"].push_back({
        {"@id", "ro-crate-metadata.json"},
        {"@type", "CreativeWork"},
        {"conformsTo", nlohmann::json::array({{ {"@id", "https://w3id.org/ro/crate/1.1"} }})},
        {"about", nlohmann::json::array({{ {"@id", "./"} }})}
        });

    // Add the root data entity
    crate_["@graph"].push_back({{"@id", "./"}, {"@type", "Dataset"}});
  }

  const nlohmann::json& crate() const
  {
    return crate_;
  }

private:
  nlohmann::json crate_;
};
