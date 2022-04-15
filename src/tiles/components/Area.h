#ifndef SECTION_H
#define SECTION_H

#include <set>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/components/Border.h>

namespace tile
{
class Area
{
public:
  Area(std::set<Border> borders);
  Area(std::set<Border> borders, std::set<Border> roads,
       std::shared_ptr<building::Building> building, portable::Cache resources);
  Area(const Area &other);
  Area();
  virtual ~Area();

  /// Clears the area of all roads/buildings/resources
  void clear();

  /// Resets the area for a new round
  void reset();

  Area operator=(const Area &other);
  Area operator+(const Area &other) const;
  Area operator+(const std::set<Border> borders) const;
  Area operator+(const portable::Cache resources) const;
  Area
  operator+(std::map<portable::Resource::Type, std::vector<portable::Resource>>
                res_list) const;

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
  void operator+=(portable::Cache const resources);
  void
  operator+=(std::map<portable::Resource::Type, std::vector<portable::Resource>>
                 res_list);

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
  inline std::map<portable::Resource::Type, std::vector<portable::Resource>>
  get_resources()
  {
    return m_resources.all();
  }
  inline std::map<portable::Resource::Type, std::vector<portable::Resource>>
  get_moveable_resources(const player::Color player)
  {
    return m_resources.all_moveable(player);
  }
  inline uint16_t get_resource_amount(const portable::Resource::Type res)
  {
    return m_resources.count(res);
  }
  inline uint16_t get_resource_amount(const portable::Resource::Type res) const
  {
    return m_resources.count(res);
  }

  inline bool has_resources() const { return m_resources.size() > 0; }

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

  /// Adds new resources of the specified type to the area
  /// @param[in] res_type  Type of resource to add
  /// @param[in] amount  Amount to add to the area
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if the input is invalid
  common::Error add_resource(const portable::Resource::Type res_type,
                             const uint16_t amount = 1);

  /// Adds resource to the area
  /// @param[in] res Resource to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if the input is invalid
  common::Error add_resource(const portable::Resource res)
  {
    return m_resources.add(res);
  }

  /// Adds resources to the area
  /// @param[in] res_list  Resources to add
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if the input is invalid
  common::Error add_resource(
      std::map<portable::Resource::Type, std::vector<portable::Resource>>
          &res_list)
  {
    return m_resources.add(res_list);
  }

  /// Removes resource from the area
  /// @param[in] res  Resource type to remove
  /// @param[in] amount  Amount of resource to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID on invalid resource specified
  ///   - common::ERR_FAIL if insufficient resource amount to remove
  common::Error remove_resource(const portable::Resource::Type res,
                                const uint16_t amount = 1)
  {
    return m_resources.remove(res, amount);
  }

  /// Retrieves a resource type from the area
  /// @param[in] res_type  Resource to remove
  /// @param[out] result  The retrieved list of resources
  /// @param[in] amount  Amount of resource to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL if insufficient resource amount to remove
  ///   - common::ERR_INVALID on invalid resource specified
  common::Error get_resource(const portable::Resource::Type res_type,
                             std::vector<portable::Resource> &result,
                             const uint16_t amount = 1)
  {
    return m_resources.get(res_type, result, amount);
  }

  /// Retrieves the specified amount of the input resource from the area.
  /// @param[in] res_type  The type of resource to remove
  /// @param[in] clr  The color of player requesting the resources
  /// @param[out] result  The resulting list of removed resources
  /// @param[in] amount  The amount to remove
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on insufficient resources in cache
  ///   - common::ERR_INVALID on invalid resource type requested
  common::Error get_resource(const portable::Resource::Type res_type,
                             const player::Color clr,
                             std::vector<portable::Resource> &result,
                             const uint16_t amount = 1)
  {
    return m_resources.get(res_type, clr, result, amount);
  }

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
  portable::Cache m_resources;
};
} // namespace tile

#endif