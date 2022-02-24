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

using namespace tile;

Area::Area(std::set<Border> borders)
    : m_p_id(utils::gen_uuid()), m_p_borders(borders)
{
  std::fill_n(m_p_resources, portable::RESOURCE_NAMES_SIZE, 0);
}

Area::Area(uuids::uuid id, std::set<Border> borders, std::set<Border> roads,
           std::shared_ptr<building::Building> building,
           uint16_t resources[portable::RESOURCE_NAMES_SIZE],
           std::vector<std::shared_ptr<portable::Transporter>>
               transporters[player::MAX_PLAYER_COLORS])
    : m_p_id(id), m_p_roads(roads), m_p_borders(borders), m_p_building(building)
{
  for (uint8_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    m_p_resources[i] = resources[i];
  }
  for (uint8_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    std::copy(transporters[c].begin(), transporters[c].end(),
              m_p_transporters[c].begin());
  }
}

Area::Area(const Area &other)
    : m_p_id(other.m_p_id), m_p_borders(other.m_p_borders),
      m_p_roads(other.m_p_roads), m_p_building(other.m_p_building),
      m_p_transporters(other.m_p_transporters)
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
  m_p_building.reset();
  std::fill_n(m_p_resources, portable::RESOURCE_NAMES_SIZE, 0);
};

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

    for (size_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
    {
      for (auto t : other.m_p_transporters[c])
      {
        if (std::find(merged.m_p_transporters[c].begin(),
                      merged.m_p_transporters[c].end(),
                      t) == merged.m_p_transporters[c].end())
        {
          merged.m_p_transporters[c].push_back(t);
        }
      }
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

  for (int c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    if (!m_p_transporters[c].empty())
    {
      std::vector<std::shared_ptr<portable::Transporter>> player_transporters;
      std::copy(m_p_transporters[c].begin(), m_p_transporters[c].end(),
                player_transporters.begin());
      new_area.m_p_transporters[c] = player_transporters;
    }
  }
  return new_area;
}

Area Area::
operator+(const uint16_t resources[portable::RESOURCE_NAMES_SIZE]) const
{
  Area new_area(m_p_borders);
  new_area.m_p_id = m_p_id;
  new_area.m_p_building = m_p_building;
  new_area.m_p_roads.insert(m_p_roads.begin(), m_p_roads.end());
  for (uint8_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    new_area.m_p_resources[i] = m_p_resources[i] + resources[i];
  }
  for (int c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    if (!m_p_transporters[c].empty())
    {
      std::vector<std::shared_ptr<portable::Transporter>> player_transporters;
      std::copy(m_p_transporters[c].begin(), m_p_transporters[c].end(),
                player_transporters.begin());
      new_area.m_p_transporters[c] = player_transporters;
    }
  }
  return new_area;
}

bool Area::operator==(Area &other)
{
  bool are_equal = (m_p_id == other.m_p_id);

  are_equal &= (other.m_p_borders == m_p_borders);
  are_equal &= (other.m_p_roads == m_p_roads);
  if (are_equal)
  {
    for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
    {
      if (m_p_resources[i] != other.m_p_resources[i])
      {
        are_equal = false;
        break;
      }
    }
    if (are_equal)
    {
      for (int i = 0; i < player::MAX_PLAYER_COLORS; i++)
      {
        if (m_p_transporters[i].size() != other.m_p_transporters[i].size())
        {
          are_equal = false;
          break;
        }
        else
        {
          for (auto t : m_p_transporters[i])
          {
            if (std::find(other.m_p_transporters[i].begin(),
                          other.m_p_transporters[i].end(),
                          t) == other.m_p_transporters[i].end())
            {
              are_equal = false;
              break;
            }
          }
        }
      }
    }
  }
  return are_equal;
}
bool Area::operator==(Area const &other) const
{
  bool are_equal = (m_p_id == other.m_p_id);

  are_equal &= (other.m_p_borders == m_p_borders);
  are_equal &= (other.m_p_roads == m_p_roads);
  if (are_equal)
  {
    for (int i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
    {
      if (m_p_resources[i] != other.m_p_resources[i])
      {
        are_equal = false;
        break;
      }
    }
    if (are_equal)
    {
      for (int i = 0; i < player::MAX_PLAYER_COLORS; i++)
      {
        if (m_p_transporters[i].size() != other.m_p_transporters[i].size())
        {
          are_equal = false;
          break;
        }
        else
        {
          for (auto t : m_p_transporters[i])
          {
            if (std::find(other.m_p_transporters[i].begin(),
                          other.m_p_transporters[i].end(),
                          t) == other.m_p_transporters[i].end())
            {
              are_equal = false;
              break;
            }
          }
        }
      }
    }
  }
  return are_equal;
}
bool Area::operator!=(Area &other) { return !(*this == other); }
bool Area::operator!=(Area const &other) const { return !(*this == other); }
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

    for (size_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
    {
      player::Color idx = static_cast<player::Color>(c);
      std::vector<std::shared_ptr<portable::Transporter>> others =
          other.m_p_transporters[idx];
      for (auto t : others)
      {
        if (std::find(m_p_transporters[idx].begin(),
                      m_p_transporters[idx].end(),
                      t) == m_p_transporters[idx].end())
        {
          std::shared_ptr<portable::Transporter> ptr(t.get());
          m_p_transporters[idx].push_back(ptr);
        }
      }
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

bool Area::does_share_direction(const Direction dir)
{
  bool retval = false;
  std::set<Direction> checked;

  for (Border b : m_p_borders)
  {
    Direction border_direction = direction_from_border(b);
    if (!checked.contains(border_direction))
    {
      if (border_direction == dir)
      {
        retval = true;
        break;
      }
      else
      {
        checked.insert(border_direction);
      }
    }
  }
  return retval;
}

common::Error Area::get_player_transporters(
    const player::Color color,
    std::vector<std::shared_ptr<portable::Transporter>> &transporters)
{
  common::Error err = common::ERR_NONE;
  if ((0 > color) || (player::MAX_PLAYER_COLORS <= color) ||
      (player::Color::neutral == color))
  {
    err = common::ERR_INVALID;
  }
  else
  {
    transporters.clear();
    std::copy(m_p_transporters[color].begin(), m_p_transporters[color].end(),
              transporters.begin());
  }

  return err;
}

bool Area::can_build_road(const Border b)
{
  bool retval = true;
  if ((!m_p_borders.contains(b)) || (m_p_roads.contains(b)))
  {
    retval = false;
  }
  else
  {
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
      retval = false;
    }
  }

  return retval;
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
  common::Error err = common::ERR_NONE;
  if (!bldg)
  {
    err = common::ERR_INVALID;
  }
  else if (m_p_building)
  {
    err = common::ERR_FAIL;
  }
  else
  {
    m_p_building = bldg;
  }
  return err;
}

common::Error Area::build(const Border border)
{
  common::Error err = common::ERR_NONE;
  if (!is_valid(border))
  {
    err = common::ERR_INVALID;
  }
  else if (!can_build_road(border))
  {
    err = common::ERR_FAIL;
  }
  else
  {
    m_p_roads.insert(border);
  }
  return err;
}

common::Error Area::add_resource(const portable::Resource res,
                                 const uint16_t amount)
{
  if (portable::is_valid(res))
  {
    m_p_resources[res] += amount;
  }
}

common::Error
Area::add_transporter(const player::Color color,
                      std::shared_ptr<portable::Transporter> transporter)
{
  common::Error err = common::ERR_NONE;
  if ((!transporter) || (!is_valid(color)))
  {
    err = common::ERR_INVALID;
  }
  else
  {
    m_p_transporters[color].push_back(transporter);
  }
  return err;
}

common::Error Area::merge(Area &other)
{
  common::Error err = common::ERR_NONE;
  if (can_merge(other))
  {
    (*this) += other;
  }
  else
  {
    err = common::ERR_FAIL;
  }
  return err;
}

void Area::rotate(int8_t rotations)
{
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

    std::vector<Border> tmp_roads;
    for (auto b : m_p_roads)
    {
      Border rotated = static_cast<Border>((rotations + b) % MAX_BORDERS);
      tmp_roads.push_back(rotated);
    }
    m_p_roads.clear();
    m_p_roads.insert(tmp_roads.begin(), tmp_roads.end());
  }
}

std::ostream &operator<<(std::ostream &os, Area &a)
{
  // TODO: Add logic to print area object
  return os;
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

  // List transporters
  for (uint8_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    std::string color = to_string(static_cast<player::Color>(c));
    bool added_transporters = false;
    if (!((m_p_transporters[c]).empty()))
    {
      for (auto transporter : m_p_transporters[c])
      {
        if (transporter)
        {
          retval[c].push_back(transporter->to_json());
          added_transporters = true;
        }
      }
    }
    if (!added_transporters)
    {
      retval[c] = std::vector<nlohmann::json>();
    }
  }

  return retval;
}

// TODO: implement from_json