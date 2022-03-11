#include <algorithm>
#include <assert.h>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/Transporter.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <utils/id_utils.h>

namespace tile
{

Area::Area(std::set<Border> borders)
    : m_p_id(utils::gen_uuid()), m_p_borders(borders)
{
  std::fill_n(m_p_resources, portable::RESOURCE_NAMES_SIZE, 0);
}

Area::Area(uuids::uuid id, std::set<Border> borders, std::set<Border> roads,
           std::shared_ptr<building::Building> building,
           uint16_t resources[portable::RESOURCE_NAMES_SIZE])
    : m_p_id(id), m_p_roads(roads), m_p_borders(borders), m_p_building(building)
{
  for (uint8_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    m_p_resources[i] = resources[i];
  }
}

Area::Area(const Area &other)
    : m_p_id(other.m_p_id), m_p_borders(other.m_p_borders),
      m_p_roads(other.m_p_roads), m_p_building(other.m_p_building)
{
  for (uint8_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    m_p_resources[i] = other.m_p_resources[i];
  }
}

Area::Area() : m_p_id(utils::gen_uuid()) {}

Area::~Area()
{
  m_p_borders.clear();
  reset();
};

void Area::reset()
{
  m_p_roads.clear();
  m_p_building.reset();
  std::fill_n(m_p_resources, portable::RESOURCE_NAMES_SIZE, 0);
}

Area Area::operator=(const Area &other)
{
  m_p_id = other.m_p_id;
  m_p_borders = other.m_p_borders;
  m_p_roads = other.m_p_roads;
  m_p_building = other.m_p_building;
  for (size_t i = 0; i < portable::RESOURCE_TYPES; i++)
  {
    m_p_resources[i] = other.m_p_resources[i];
  }
  return (*this);
}

Area Area::operator+(const Area &other) const
{
  Area merged = Area(*this);
  if (merged.can_merge(other))
  {
    merged.m_p_borders.insert(other.m_p_borders.begin(),
                              other.m_p_borders.end());
    merged.m_p_roads.insert(other.m_p_roads.begin(), other.m_p_roads.end());

    if (!merged.m_p_building)
    {
      merged.m_p_building = other.m_p_building;
    }

    for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
    {
      merged.m_p_resources[i] += other.m_p_resources[i];
    }
  }

  return merged;
}

Area Area::operator+(const std::set<Border> borders) const
{
  std::set<Border> merged_borders;
  std::merge(m_p_borders.begin(), m_p_borders.end(), borders.begin(),
             borders.end(),
             std::inserter(merged_borders, merged_borders.begin()));

  Area new_area(merged_borders);
  new_area.m_p_id = m_p_id;
  new_area.m_p_building = m_p_building;
  new_area.m_p_roads.insert(m_p_roads.begin(), m_p_roads.end());
  std::copy(std::begin(m_p_resources), std::end(m_p_resources),
            std::begin(new_area.m_p_resources));
  return new_area;
}

Area Area::
operator+(const uint16_t resources[portable::RESOURCE_NAMES_SIZE]) const
{
  Area new_area(m_p_borders);
  new_area.m_p_id = m_p_id;
  new_area.m_p_building = m_p_building;
  new_area.m_p_roads.insert(m_p_roads.begin(), m_p_roads.end());
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    new_area.m_p_resources[i] = m_p_resources[i] + resources[i];
  }
  return new_area;
}

bool Area::operator==(Area &other)
{
  if ((m_p_id != other.m_p_id) || (other.m_p_borders != m_p_borders) ||
      (other.m_p_roads != m_p_roads) || (other.m_p_building != m_p_building))
  {
    return false;
  }
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    if (m_p_resources[i] != other.m_p_resources[i])
    {
      return false;
    }
  }
  return true;
}
bool Area::operator==(Area const &other) const
{
  if ((m_p_id != other.m_p_id) || (other.m_p_borders != m_p_borders) ||
      (other.m_p_roads != m_p_roads) || (other.m_p_building != m_p_building))
  {
    return false;
  }
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    if (m_p_resources[i] != other.m_p_resources[i])
    {
      return false;
    }
  }
  return true;
}
bool Area::operator==(std::set<Border> &borders)
{
  return m_p_borders == borders;
}
bool Area::operator==(std::set<Border> const &borders) const
{
  return m_p_borders == borders;
}
bool Area::operator!=(Area &other) { return !(*this == other); }
bool Area::operator!=(Area const &other) const { return !(*this == other); }
bool Area::operator!=(std::set<Border> &borders) { return !(*this == borders); }
bool Area::operator!=(std::set<Border> const &borders) const
{
  return !(*this == borders);
}
bool Area::operator<(Area const &other) const
{
  return uuids::to_string(m_p_id) < uuids::to_string(other.m_p_id);
}
bool Area::operator<(Area &other)
{
  return uuids::to_string(m_p_id) < uuids::to_string(other.m_p_id);
}
bool Area::operator>(Area const &other) const
{
  return uuids::to_string(m_p_id) > uuids::to_string(other.m_p_id);
}
bool Area::operator>(Area &other)
{
  return uuids::to_string(m_p_id) > uuids::to_string(other.m_p_id);
}

void Area::operator+=(Area const &other)
{
  if (can_merge(other))
  {
    m_p_borders.insert(other.m_p_borders.begin(), other.m_p_borders.end());
    m_p_roads.insert(other.m_p_roads.begin(), other.m_p_roads.end());

    if (nullptr == m_p_building)
    {
      m_p_building = other.m_p_building;
    }

    for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
    {
      m_p_resources[i] += other.m_p_resources[i];
    }
  }
}

void Area::operator+=(std::set<Border> const borders)
{
  std::merge(m_p_borders.begin(), m_p_borders.end(), borders.begin(),
             borders.end(), std::inserter(m_p_borders, m_p_borders.begin()));
}

void Area::operator+=(uint16_t const resources[portable::RESOURCE_NAMES_SIZE])
{
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    m_p_resources[i] += resources[i];
  }
}

template <typename Iter> bool Area::has_borders(Iter begin, Iter end)
{
  while (begin != end)
  {
    if (!m_p_borders.contains(*begin))
    {
      return false;
    }
    ++begin;
  }
  return true;
}

bool Area::has_resources() const
{
  for (size_t r = 0; r < portable::RESOURCE_NAMES_SIZE; r++)
  {
    if (m_p_resources[r] > 0)
    {
      return true;
    }
  }
  return false;
}
std::map<portable::Resource, uint16_t> Area::list_available_resources() const
{
  std::map<portable::Resource, uint16_t> retval;
  for (size_t r = 0; r < portable::RESOURCE_NAMES_SIZE; r++)
  {
    if (m_p_resources[r] > 0)
    {
      retval.at(static_cast<portable::Resource>(r)) = m_p_resources[r];
    }
  }
  return retval;
}

bool Area::contains(const Area other)
{
  return has_borders<std::set<Border>::iterator>(other.m_p_borders.begin(),
                                                 other.m_p_borders.end());
}

bool Area::does_share_direction(const Direction dir)
{
  std::set<Direction> checked;

  for (Border b : m_p_borders)
  {
    Direction border_direction = direction_from_border(b);
    if ((border_direction == dir) && (!checked.contains(border_direction)))
    {
      return true;
    }
    checked.insert(border_direction);
  }
  return false;
}

bool Area::can_build_road(const Border b)
{
  if ((!m_p_borders.contains(b)) || (m_p_roads.contains(b)))
  {
    return false;
  }

  Direction d = direction_from_border(b);
  std::vector<Border> both = borders_from_direction(d);
  bool has_both_borders = ((m_p_borders.contains(both.at(0))) &&
                           (m_p_borders.contains(both.at(1))));
  bool direction_has_road =
      ((m_p_roads.contains(both.at(0))) || (m_p_roads.contains(both.at(1))));

  // If the border isn't split by a river, only allow 1 road to be built
  // between both borders in that direction.
  if (has_both_borders && direction_has_road)
  {
    return false;
  }

  return true;
}

bool Area::can_merge(Area &other)
{
  return (((*this) != other) &&
          ((nullptr == m_p_building) || (nullptr == other.m_p_building)));
}

bool Area::can_merge(Area const &other) const
{
  return (((*this) != other) &&
          ((nullptr == m_p_building) || (nullptr == other.m_p_building)));
}

common::Error Area::build(std::shared_ptr<building::Building> bldg)
{
  if (!bldg)
  {
    return common::ERR_INVALID;
  }

  if (m_p_building)
  {
    return common::ERR_FAIL;
  }

  m_p_building = bldg;
  return common::ERR_NONE;
}

common::Error Area::build(const Border border)
{
  if (!is_valid(border))
  {
    return common::ERR_INVALID;
  }
  if (!can_build_road(border))
  {
    return common::ERR_FAIL;
  }

  m_p_roads.insert(border);
  return common::ERR_NONE;
}

common::Error Area::add_resource(const portable::Resource res,
                                 const uint16_t amount)
{
  if (!portable::is_valid(res))
  {
    return common::ERR_INVALID;
  }

  m_p_resources[res] += amount;
  return common::ERR_NONE;
}

common::Error Area::merge(Area &other)
{
  common::Error err = common::ERR_NONE;
  if (can_merge(other))
  {
    (*this) += other;
    return common::ERR_NONE;
  }

  return common::ERR_FAIL;
}

bool Area::can_rotate() const
{
  if ((0 != m_p_roads.size()) || (m_p_building))
  {
    return false;
  }
  for (auto res : m_p_resources)
  {
    if (0 != res)
    {
      return false;
    }
  }
  return true;
}

common::Error Area::rotate(int rotations)
{
  if (!can_rotate())
  {
    return common::ERR_FAIL;
  }

  // Making 0 rotations doesn't do anything...
  if (0 != rotations)
  {
    bool is_clockwise = (0 < rotations);
    rotations = abs(rotations) % MAX_DIRECTIONS;
    rotations = (is_clockwise ? rotations : (MAX_DIRECTIONS - rotations));
    // Two borders makes up one direction. Rotating an area is done by
    // direction; hence rotating for two borders.
    rotations *= 2;

    std::vector<Border> tmp_borders;
    for (auto b : m_p_borders)
    {
      Border rotated = static_cast<Border>((rotations + b) % MAX_BORDERS);
      tmp_borders.push_back(rotated);
    }
    m_p_borders.clear();
    m_p_borders.insert(tmp_borders.begin(), tmp_borders.end());
  }

  return common::ERR_NONE;
}

nlohmann::json Area::to_json()
{
  nlohmann::json retval;
  retval["id"] = uuids::to_string(m_p_id);
  // List borders
  for (auto border : m_p_borders)
  {
    nlohmann::json j;
    j["name"] = to_string(border);
    j["has_road"] = (m_p_roads.contains(border));
    retval["borders"].push_back(j);
  }

  // List building if found
  if (m_p_building)
  {
    retval["building"] = m_p_building->to_json();
  }
  else
  {
    retval["building"] = nullptr;
  }

  // List resources
  for (uint8_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource r = static_cast<portable::Resource>(i);
    retval["resources"][to_string(r)] = m_p_resources[i];
  }

  return retval;
}

std::ostream &operator<<(std::ostream &os, tile::Area const &a)
{
  std::set<tile::Border> bdrs = a.get_borders();
  std::set<tile::Border> rds = a.get_roads();
  std::vector<tile::Border> borders(bdrs.begin(), bdrs.end());
  std::vector<tile::Border> roads(rds.begin(), rds.end());
  os << "<Area::id=" << a.get_id() << ", borders=[" << borders.at(0);
  for (size_t i = 1; i < borders.size(); i++)
  {
    os << ", " << borders.at(i);
  }
  os << "]";

  if (roads.size() > 0)
  {
    os << ", roads=[";
    os << roads.at(0);
    for (size_t i = 1; i < roads.size(); i++)
    {
      os << ", " << roads.at(i);
    }
    os << "]";
  }

  if (a.get_building())
  {
    os << ", building=";
    os << (*a.get_building()).get_name();
  }

  if (a.has_resources())
  {
    std::map<portable::Resource, uint16_t> res = a.list_available_resources();
    os << ", resources={";
    for (std::map<portable::Resource, uint16_t>::iterator it = res.begin();
         it != res.end(); ++it)
    {
      os << "{" << it->first << ":" << static_cast<int>(it->second) << "}";
    }
    os << "}";
  }
  os << ">";
  return os;
}
} // namespace tile
  // TODO: implement from_json