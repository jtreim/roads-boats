#ifndef SECTION_H
#define SECTION_H

#include <set>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/Transporter.h>
#include <tiles/components/Border.h>

namespace tile
{
class Area
{
public:
  Area(std::set<Border> borders);
  Area(uuids::uuid id, std::set<Border> borders,
       std::shared_ptr<building::Building> building,
       uint16_t resources[portable::RESOURCE_NAMES_SIZE],
       std::vector<std::shared_ptr<portable::Transporter>>
           transporters[player::MAX_PLAYER_COLORS]);
  Area(const Area &other);
  Area();
  virtual ~Area();

  Area operator=(const Area &other);
  Area operator+(const Area &other) const;
  Area operator+(const std::set<Border> borders) const;
  Area operator+(const uint16_t resources[portable::RESOURCE_NAMES_SIZE]) const;

  bool operator==(Area &other);
  bool operator==(Area const &other) const;
  bool operator!=(Area &other);
  bool operator!=(Area const &other) const;
  bool operator<(Area const &other) const;
  bool operator<(Area &other);
  bool operator>(Area const &other) const;
  bool operator>(Area &other);

  void operator+=(Area const &other);
  void operator+=(std::set<Border> const borders);
  void operator+=(uint16_t const resources[portable::RESOURCE_NAMES_SIZE]);

  inline uuids::uuid get_id() const { return m_p_id; }

  /// Checks to see if the input border surrounds this area
  /// @param[in] b
  /// @param[out] has_border The result of the query
  /// @return
  ///   - common::ERR_NONE on successful check
  ///   - common::ERR_INVALID on invalid border input
  common::Error has_border(const Border b, bool &has_border);

  /// Checks to see if one of the area's borders is in the input direction
  /// @param[in] d
  /// @param[out] shares_direction The result of the query
  /// @return
  ///   - common::ERR_NONE on successful check
  ///   - common::ERR_INVALID on invalid direction input
  common::Error does_share_direction(const Direction d, bool &shares_direction);

  /// Retrieves all transporters in the area for the input player
  /// @param[in] color Color of the player to search
  /// @param[out] transporters Resulting list of transporters
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid inputs
  common::Error get_player_transporters(
      const player::Color color,
      std::vector<std::shared_ptr<portable::Transporter>> &transporters);

  /// Retrieves the building in this area
  /// @return ptr to the building; null on none
  std::shared_ptr<building::Building> get_building();

  bool can_merge(Area &other);
  bool can_merge(Area const &other) const;

  /// Combines this area with another to make a single area.
  /// @param other Area to be merged with
  /// @return
  ///   - common::ERR_INVALID on invalid input
  ///   - common::ERR_FAIL on failure to merge areas
  ///   - common::ERR_NONE on success. This area will be merged with other
  common::Error merge(Area &other);

  friend std::ostream &operator<<(std::ostream &os, Area &a);
  nlohmann::json to_json();

protected:
private:
  uuids::uuid m_p_id;
  std::set<Border> m_p_borders;
  std::shared_ptr<building::Building> m_p_building;
  uint16_t m_p_resources[portable::RESOURCE_NAMES_SIZE];
  std::vector<std::shared_ptr<portable::Transporter>>
      m_p_transporters[(player::MAX_PLAYER_COLORS)];
};

/// Create an Area object using the input json.
/// @param[in] j Input json
/// @param[out] a Pointer to created Area object. Null on invalid json input.
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on invalid json object
static common::Error from_json(const nlohmann::json j,
                               std::shared_ptr<Area> &a);
} // namespace tile

#endif