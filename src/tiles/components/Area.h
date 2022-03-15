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
  Area(std::set<Border> borders, std::set<Border> roads,
       std::shared_ptr<building::Building> building,
       uint16_t resources[portable::RESOURCE_NAMES_SIZE]);
  Area(const Area &other);
  Area();
  virtual ~Area();

  // Clears the area of all roads/buildings/resources
  void reset();

  Area operator=(const Area &other);
  Area operator+(const Area &other) const;
  Area operator+(const std::set<Border> borders) const;
  Area operator+(const uint16_t resources[portable::RESOURCE_NAMES_SIZE]) const;

  bool operator==(std::set<Border> &borders);
  bool operator==(std::set<Border> const &borders) const;
  bool operator==(Area &other);
  bool operator==(Area const &other) const;
  bool operator!=(std::set<Border> &borders);
  bool operator!=(std::set<Border> const &borders) const;
  bool operator!=(Area &other);
  bool operator!=(Area const &other) const;
  bool operator<(Area const &other) const;
  bool operator<(Area &other);
  bool operator>(Area const &other) const;
  bool operator>(Area &other);

  void operator+=(Area const &other);
  void operator+=(std::set<Border> const borders);
  void operator+=(uint16_t const resources[portable::RESOURCE_NAMES_SIZE]);

  inline bool has_border(const Border b) { return m_borders.contains(b); }
  template <typename Iter> bool has_borders(Iter begin, Iter end);
  inline bool has_road(const Border b) { return m_roads.contains(b); }
  inline std::set<Border> get_borders() { return m_borders; }
  inline std::set<Border> get_borders() const { return m_borders; }
  inline std::set<Border> get_roads() { return m_roads; }
  inline std::set<Border> get_roads() const { return m_roads; }
  inline std::shared_ptr<building::Building> get_building()
  {
    return m_building;
  };
  inline std::shared_ptr<building::Building> get_building() const
  {
    return m_building;
  };
  inline uint16_t *get_resources() { return m_resources; }
  inline uint16_t get_resource_amount(const portable::Resource res)
  {
    if (portable::is_valid(res))
    {
      return m_resources[res];
    }
    return 0;
  }
  inline uint16_t get_resource_amount(const portable::Resource res) const
  {
    if (portable::is_valid(res))
    {
      return m_resources[res];
    }
    return 0;
  }

  bool has_resources() const;
  std::map<portable::Resource, uint16_t> list_available_resources() const;

  /// Checks to see if input Area is contained within this Area.
  /// @param[in] other
  /// @return true, false
  bool contains(const Area other);

  /// Checks to see if one of the area's borders is in the input direction
  /// @param[in] dir
  /// @return boolean for whether the area does include the input direction
  bool does_share_direction(const Direction dir);

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

  /// Combines this area with another to make a single area.
  /// @param other Area to be merged with
  /// @return
  ///   - common::ERR_INVALID on invalid input
  ///   - common::ERR_FAIL on failure to merge areas
  ///   - common::ERR_NONE on success. This area will be merged with other
  common::Error merge(Area &other);

  bool can_rotate() const;

  /// Rotates the area clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise. This value is the same # of
  /// rotations that the tile should make as a whole.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failed can_rotate check
  ///   - common::ERR_UNKNOWN otherwise
  common::Error rotate(int rotations);

  friend std::ostream &operator<<(std::ostream &os, tile::Area const &a);
  friend void to_json(nlohmann::json &j, const Area &area);
  friend void from_json(const nlohmann::json &j, Area &area);

protected:
private:
  std::set<Border> m_borders;
  std::set<Border> m_roads;
  std::shared_ptr<building::Building> m_building;
  uint16_t m_resources[portable::RESOURCE_NAMES_SIZE];
};
} // namespace tile

#endif