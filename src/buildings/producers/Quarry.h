#ifndef QUARRY_H
#define QUARRY_H

#include <memory>

#include <nlohmann/json.hpp>

#include <buildings/Primary.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
class Quarry : public Primary_producer
{
public:
  Quarry();
  Quarry(const Quarry &other);
  ~Quarry();

  Quarry operator=(const Quarry &other);

  /// Produces resource/transporter with the input resources.
  /// @param[in] input  Resources building can use to produce.
  /// @param[in] nearby_transporters  Transporters that currently have access to
  /// this building.
  /// @param[out] output  Result of production. null on failure to produce
  /// @return
  ///   - common::ERR_NONE on success.
  ///   - common::ERR_INVALID on invalid resource or transporter input.
  ///   - common::ERR_FAIL on insufficient resources given as input.
  common::Error
  produce(portable::Cache &input,
          std::vector<portable::Transporter *> &nearby_transporters,
          std::vector<portable::Portable *> &output);

  /// Determines whether the building can be placed on the input tile. Does not
  /// check tile for existing buildings, or if the constructing transporter has
  /// access to a specific area.
  /// @param[in] input  Resources available for constructing the building
  /// @param[in] tile  tile to be placing the building
  static bool can_build(const portable::Cache &input, const tile::Tile *tile);

  static common::Error remove_construction_resources(portable::Cache &input);

  // helpers
  std::string to_string() const;
  nlohmann::json to_json() const;
  void from_json(nlohmann::json const &j);

protected:
private:
};
} // namespace building

#endif