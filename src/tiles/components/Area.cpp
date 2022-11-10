#include <algorithm>
#include <assert.h>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <buildings/utils.h>

#include <common/Errors.h>
#include <players/Color.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <utils/id_utils.h>

namespace tile
{
Area::Area(std::set<Border> borders, tile::Tile *parent)
    : m_borders(borders), m_parent(parent)
{
}

Area::Area(std::set<Border> borders, std::set<Border> roads,
           std::unique_ptr<building::Building> &building,
           portable::Cache &resources, tile::Tile *parent)
    : m_roads(roads), m_borders(borders), m_building(std::move(building)),
      m_resources(resources), m_parent(parent)
{
}

Area::Area(const Area &other)
    : m_borders(other.m_borders), m_roads(other.m_roads),
      m_building(std::move(other.m_building.get())),
      m_resources(other.m_resources), m_parent(other.m_parent)
{
}

Area::Area() {}

Area::~Area()
{
  m_borders.clear();
  reset();
};

void Area::clear()
{
  m_roads.clear();
  m_building.reset();
  m_resources.clear();
}

void Area::reset() { m_resources.reset(); }

Area Area::operator=(const Area &other)
{
  m_borders = other.m_borders;
  m_roads = other.m_roads;
  if (other.m_building)
  {
    building::Building::Type bldg_type = other.m_building->get_type();
    (void)building::make_building(bldg_type, m_building,
                                  other.m_building.get());
  }
  m_resources = other.m_resources;
  m_parent = other.m_parent;
  return (*this);
}

bool Area::operator==(Area &other)
{
  return ((other.m_borders == m_borders) && (other.m_roads == m_roads) &&
          (other.m_building == m_building) &&
          (other.m_resources == m_resources) && (other.m_parent == m_parent));
}
bool Area::operator==(Area const &other) const
{
  return ((other.m_borders == m_borders) && (other.m_roads == m_roads) &&
          (other.m_building == m_building) &&
          (other.m_resources == m_resources) && (other.m_parent == m_parent));
}
bool Area::operator==(std::set<Border> &borders)
{
  return m_borders == borders;
}
bool Area::operator==(std::set<Border> const &borders) const
{
  return m_borders == borders;
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
  bool result = false;
  if ((m_borders.size() == 0) && (other.m_borders.size() > 0))
  {
    result = true;
  }
  else if ((other.m_borders.size() == 0) && (m_borders.size() > 0))
  {
    result = false;
  }
  else if ((other.m_borders.size() > 0) && (m_borders.size() > 0))
  {
    // Compare border lists side by side. Area with the most clockwise border is
    // less.
    int i_count = 0;
    int j_count = 0;
    auto i = m_borders.begin();
    auto j = other.m_borders.begin();
    while ((i_count < m_borders.size()) && (j_count < other.m_borders.size()))
    {
      if (*i != *j)
      {
        result = (*i < *j);
        break;
      }
      ++i;
      ++j;
    }
  }

  return result;
}
bool Area::operator<(Area &other)
{
  bool result = false;
  if ((m_borders.size() == 0) && (other.m_borders.size() > 0))
  {
    result = true;
  }
  else if ((other.m_borders.size() == 0) && (m_borders.size() > 0))
  {
    result = false;
  }
  else if ((other.m_borders.size() > 0) && (m_borders.size() > 0))
  {
    // Compare border lists side by side. Area with the most clockwise border is
    // less.
    int i_count = 0;
    int j_count = 0;
    auto i = m_borders.begin();
    auto j = other.m_borders.begin();
    while ((i_count < m_borders.size()) && (j_count < other.m_borders.size()))
    {
      if (*i != *j)
      {
        result = (*i < *j);
        break;
      }
      ++i;
      ++j;
    }
  }

  return result;
}
bool Area::operator>(Area const &other) const
{
  bool result = false;
  if ((m_borders.size() == 0) && (other.m_borders.size() > 0))
  {
    result = false;
  }
  else if ((other.m_borders.size() == 0) && (m_borders.size() > 0))
  {
    result = true;
  }
  else if ((other.m_borders.size() > 0) && (m_borders.size() > 0))
  {
    // Compare border lists side by side. Area with the most clockwise border is
    // less.
    int i_count = 0;
    int j_count = 0;
    auto i = m_borders.begin();
    auto j = other.m_borders.begin();
    while ((i_count < m_borders.size()) && (j_count < other.m_borders.size()))
    {
      if (*i != *j)
      {
        result = (*i > *j);
        break;
      }
      ++i;
      ++j;
    }
  }

  return result;
}
bool Area::operator>(Area &other)
{
  bool result = false;
  if ((m_borders.size() == 0) && (other.m_borders.size() > 0))
  {
    result = false;
  }
  else if ((other.m_borders.size() == 0) && (m_borders.size() > 0))
  {
    result = true;
  }
  else if ((other.m_borders.size() > 0) && (m_borders.size() > 0))
  {
    // Compare border lists side by side. Area with the most clockwise border is
    // less.
    int i_count = 0;
    int j_count = 0;
    auto i = m_borders.begin();
    auto j = other.m_borders.begin();
    while ((i_count < m_borders.size()) && (j_count < other.m_borders.size()))
    {
      if (*i != *j)
      {
        result = (*i > *j);
        break;
      }
      ++i;
      ++j;
    }
  }

  return result;
}

void Area::operator+=(const std::vector<portable::Resource *> &res_list)
{
  m_resources += res_list;
}

template <typename Iter> bool Area::has_borders(Iter begin, Iter end)
{
  while (begin != end)
  {
    if (!m_borders.contains(*begin))
    {
      return false;
    }
    ++begin;
  }
  return true;
}

bool Area::contains(const Area other)
{
  return has_borders<std::set<Border>::iterator>(other.m_borders.begin(),
                                                 other.m_borders.end());
}

bool Area::does_share_direction(const Direction dir)
{
  std::set<Direction> checked;

  for (Border b : m_borders)
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
  if ((!m_borders.contains(b)) || (m_roads.contains(b)))
  {
    return false;
  }

  Direction d = direction_from_border(b);
  std::vector<Border> both = borders_from_direction(d);
  bool has_both_borders =
      ((m_borders.contains(both.at(0))) && (m_borders.contains(both.at(1))));
  bool direction_has_road =
      ((m_roads.contains(both.at(0))) || (m_roads.contains(both.at(1))));

  // If the border isn't split by a river, only allow 1 road to be built
  // between both borders in that direction.
  if (has_both_borders && direction_has_road)
  {
    return false;
  }

  return true;
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

  m_roads.insert(border);
  return common::ERR_NONE;
}

bool Area::can_rotate() const
{
  if ((0 != m_roads.size()) || (m_building))
  {
    return false;
  }
  return (m_resources.size() == 0);
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
    for (auto b : m_borders)
    {
      Border rotated = static_cast<Border>((rotations + b) % MAX_BORDERS);
      tmp_borders.push_back(rotated);
    }
    m_borders.clear();
    m_borders.insert(tmp_borders.begin(), tmp_borders.end());
  }

  return common::ERR_NONE;
}

common::Error Area::load_building(const nlohmann::json &j)
{
  common::Error err = common::ERR_FAIL;
  if (!j.contains("type"))
  {
    return common::ERR_INVALID;
  }

  building::Building::Type to_build = building::Building::Type::invalid;

  for (size_t i = 0; i < building::BUILDING_NAMES_SIZE; i++)
  {
    if (building::BUILDING_NAMES[i] == j.at("type").get<std::string>())
    {
      to_build = static_cast<building::Building::Type>(i);
      err = building::make_building(to_build, m_building);
      break;
    }
  }

  if ((!err) && (nullptr != m_building))
  {
    m_building->from_json(j);
    err = common::ERR_NONE;
  }
  return err;
}

std::ostream &operator<<(std::ostream &os, tile::Area const &a)
{
  std::set<tile::Border> bdrs = a.get_borders();
  std::set<tile::Border> rds = a.get_roads();
  std::vector<tile::Border> borders(bdrs.begin(), bdrs.end());
  std::vector<tile::Border> roads(rds.begin(), rds.end());
  os << "<Area::borders=[" << borders.at(0);
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
    os << (*a.get_building()).to_string();
  }

  if (a.has_resources())
  {
    os << ", resources=" << a.m_resources;
  }
  os << ">";
  return os;
}

void to_json(nlohmann::json &j, const Area &area)
{
  // List borders
  for (auto border : area.m_borders)
  {
    nlohmann::json b;
    to_json(b, border);
    j["borders"].push_back(border);
  }
  if (area.m_roads.size() == 0)
  {
    j["roads"] = nlohmann::json::array();
  }
  else
  {
    for (auto road : area.m_roads)
    {
      j["roads"].push_back(to_string(road));
    }
  }

  // List building if found
  if (nullptr != area.m_building)
  {
    j["building"] = area.m_building->to_json();
  }
  else
  {
    j["building"] = nullptr;
  }

  // List resources
  nlohmann::json res_cache;
  to_json(res_cache, area.m_resources);
  j["resources"] = res_cache;
}

void from_json(const nlohmann::json &j, Area &area)
{
  std::set<Border> borders;
  for (auto b : j.at("borders").get<std::vector<Border>>())
  {
    if (Border::invalid_border == b)
    {
      throw nlohmann::json::type_error::create(
          501, "Invalid border value given as area border!", j);
    }
    borders.insert(b);
  }

  area = Area(borders);

  for (auto r : j.at("roads").get<std::vector<Border>>())
  {
    if (common::ERR_NONE != area.build(r))
    {
      area = Area();
      throw nlohmann::json::type_error::create(
          501, "Invalid border value given as area road: " + to_string(r), j);
    }
  }

  if ((j.contains("building")) && (!j.at("building").is_null()))
  {
    building::Building *bldg;
    if (common::ERR_NONE !=
        area.load_building(j.at("building").get<nlohmann::json>()))
    {
      area = Area();
      throw nlohmann::json::type_error::create(501, "Invalid building JSON", j);
    }
  }

  area.m_resources = j.at("resources").get<portable::Cache>();
}
} // namespace tile