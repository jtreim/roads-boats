#ifndef MINE_H
#define MINE_H

#include <vector>

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
class Mine : public Primary_producer
{
public:
  enum Mine_type
  {
    invalid = -1,
    regular = 0,
    specialized_iron,
    specialized_gold,
    big
  };

  Mine();
  Mine(uint8_t gold_amount, uint8_t iron_amount);
  Mine(const Mine &other);
  ~Mine();

  Mine operator=(const Mine &other);

  /// Determines whether the building can produce with the given resources
  /// @param[in] input  List of resources available to the building
  bool
  can_produce(const portable::Cache &input,
              const std::vector<portable::Transporter *> nearby_transporters);

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
          std::vector<std::unique_ptr<portable::Portable>> &output);

  /// Returns a count of the specified resource this mine has yet to produce
  /// @param[in] t  Type of resource to count
  /// @return  Amount of the specified resource the mine will produce
  uint8_t count(const portable::Resource::Type t) const;

  /// Determines whether a mine shaft can be added with the input resources.
  /// @return  False if the mine still has resources to produce, or if the
  /// required input doesn't exist. True otherwise.
  bool can_build_shaft(const portable::Cache &input) const;

  /// Builds a mine shaft of the specified type with the input resources.
  /// @param[in] input  Resources used to build the shaft
  /// @param[in] m  Type of mine to construct
  /// @return
  ///   - common::ERR_INVALID if invalid type specified
  ///   - common::ERR_FAIL if unable to build a shaft with the input resources
  ///   - common::ERR_NONE on success
  common::Error build_shaft(portable::Cache &input,
                            const Mine_type m = Mine_type::regular);

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
  void add_to_mine(uint8_t gold_amount, uint8_t iron_amount);

  std::vector<portable::Resource::Type> m_remaining_resources;
};
} // namespace building

#endif