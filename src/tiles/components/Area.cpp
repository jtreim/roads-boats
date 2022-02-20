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
#include <utils/id_utils.h>

using namespace tile;

Area::Area(std::set<Border> borders)
    : m_p_id(utils::gen_uuid()), m_p_borders(borders)
{
  std::fill_n(m_p_resources, portable::RESOURCE_NAMES_SIZE, 0);
}

Area::Area(uuids::uuid id, std::set<Border> borders,
           std::shared_ptr<building::Building> building,
           uint16_t resources[portable::RESOURCE_NAMES_SIZE],
           std::vector<std::shared_ptr<portable::Transporter>>
               transporters[player::MAX_PLAYER_COLORS])
    : m_p_id(id), m_p_borders(borders), m_p_building(building)
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
      m_p_building(other.m_p_building), m_p_transporters(other.m_p_transporters)
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

Area Area::operator=(const Area &other) { return Area(other); }

Area Area::operator+(const Area &other) const
{
  Area merged = Area(other);
  if (merged.can_merge(other))
  {
    std::merge(m_p_borders.begin(), m_p_borders.end(),
               merged.m_p_borders.begin(), merged.m_p_borders.end(),
               std::inserter(merged.m_p_borders, merged.m_p_borders.begin()));

    if (nullptr == merged.m_p_building)
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

bool Area::operator==(Area &other) { return m_p_id == other.m_p_id; }
bool Area::operator==(Area const &other) const
{
  return m_p_id == other.m_p_id;
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
  std::merge(m_p_borders.begin(), m_p_borders.end(), other.m_p_borders.begin(),
             other.m_p_borders.end(),
             std::inserter(m_p_borders, m_p_borders.begin()));

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
      if (std::find(m_p_transporters[idx].begin(), m_p_transporters[idx].end(),
                    t) == m_p_transporters[idx].end())
      {
        std::shared_ptr<portable::Transporter> ptr(t.get());
        m_p_transporters[idx].push_back(ptr);
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

common::Error Area::has_border(const Border b, bool &has_border)
{
  common::Error err = common::ERR_NONE;
  has_border = false;
  if ((0 > b) || (MAX_BORDERS <= b))
  {
    err = common::ERR_INVALID;
  }
  else
  {
    has_border = (std::find(m_p_borders.begin(), m_p_borders.end(), b) !=
                  m_p_borders.end());
  }

  return err;
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

std::shared_ptr<building::Building> Area::get_building()
{
  return m_p_building;
}

bool Area::can_merge(Area &other)
{
  return (!(((*this) == other) ||
            ((nullptr != m_p_building) && (nullptr != other.m_p_building) &&
             (m_p_building != other.m_p_building))));
}

bool Area::can_merge(Area const &other) const
{
  return (!(((*this) == other) ||
            ((nullptr != m_p_building) && (nullptr != other.m_p_building) &&
             (m_p_building != other.m_p_building))));
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
    retval["borders"].push_back(json(border));
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