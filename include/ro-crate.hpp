#pragma once

#include <nlohmann/json.hpp>

namespace rocrate
{

using json = nlohmann::json;

class ROCrate
{

public:
  ROCrate(): crate_(nlohmann::json::object())
  {
    // Add the context entry
    crate_["@context"] = "https://w3id.org/ro/crate/1.1/context";

    // Add the metadata file descriptor
    json metadata = json::object({
        {"@id", "ro-crate-metadata.json"},
        {"@type", "CreativeWork"},
        {"conformsTo", {{"@id", "https://w3id.org/ro/crate/1.1"}}},
        {"about", {{"@id", "./"}}}
    });

    // Add the root data entity
    json root = json::object({
        {"@id", "./"},
        {"@type", "Dataset"}
    });

    crate_["@graph"] = json::array({ metadata, root });
  }

  const nlohmann::json& crate() const
  {
    return crate_;
  }

private:
  nlohmann::json crate_;
};

} // namespace rocrate
