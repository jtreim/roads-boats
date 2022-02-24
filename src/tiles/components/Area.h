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
  Area(uuids::uuid id, std::set<Border> borders, std::set<Border> roads,
       std::shared_ptr<building::Building> building,
       uint16_t resources[portable::RESOURCE_NAMES_SIZE],
       std::vector<std::shared_ptr<portable::Transporter>>
           transporters[player::MAX_PLAYER_COLORS]);
  Area(const Area &other);
  Area();
  virtual ~Area();

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
  inline bool has_border(const Border b) { return m_p_borders.contains(b); }
  inline bool has_road(const Border b) { return m_p_roads.contains(b); }
  inline std::set<Border> get_borders() { return m_p_borders; }
  inline std::set<Border> get_roads() { return m_p_roads; }
  inline std::shared_ptr<building::Building> get_building()
  {
    return m_p_building;
  };
  inline uint16_t *get_all_resources() { return m_p_resources; }
  inline uint16_t get_resource_amount(const portable::Resource res)
  {
    if (portable::is_valid(res))
    {
      return m_p_resources[res];
    }
    return 0;
  }

  /// Checks to see if one of the area's borders is in the input direction
  /// @param[in] dir
  /// @return boolean for whether the area does include the input direction
  bool does_share_direction(const Direction dir);

  /// Retrieves all transporters in the area for the input player
  /// @param[in] color Color of the player to search
  /// @param[out] transporters Resulting list of transporters
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid inputs
  common::Error get_player_transporters(
      const player::Color color,
      std::vector<std::shared_ptr<portable::Transporter>> &transporters);

  bool can_build_road(const Border b);
  bool can_merge(Area &other);
  bool can_merge(Area const &other) const;

  /// Adds building to this area
  /// @param[in] bldg Desired building to add to the area.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if bldg is null
  ///   - common::ERR_FAIL otherwise
  common::Error build(std::shared_ptr<building::Building> bldg);

  /// Adds road to the input border
  /// @param[in] border Desired border to build a road
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if border is invalid value
  ///   - common::ERR_FAIL if unable to build road on this border
  common::Error build(const Border border);

  /// Adds resource to the area
  /// @param[in] res Resource to add
  /// @param[in] amount Amount of resource to add.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if the input is invalid
  ///   - common::ERR_FAIL if unable to add resource
  common::Error add_resource(const portable::Resource res,
                             const uint16_t amount = 1);

  /// Moves transporter into this area
  /// @param[in] color Player owning the transporter
  /// @param[in] transporter Transporter to be added to the area
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if transporter is null or color is invalid
  ///   - common::ERR_FAIL otherwise
  common::Error
  add_transporter(const player::Color color,
                  std::shared_ptr<portable::Transporter> transporter);

  /// Combines this area with another to make a single area.
  /// @param other Area to be merged with
  /// @return
  ///   - common::ERR_INVALID on invalid input
  ///   - common::ERR_FAIL on failure to merge areas
  ///   - common::ERR_NONE on success. This area will be merged with other
  common::Error merge(Area &other);

  /// Rotates the area clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise. This value is the same # of
  /// rotations that the tile should make as a whole.
  void rotate(int8_t rotations);

  friend std::ostream &operator<<(std::ostream &os, Area &a);
  nlohmann::json to_json();

protected:
private:
  uuids::uuid m_p_id;
  std::set<Border> m_p_borders;
  std::set<Border> m_p_roads;
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