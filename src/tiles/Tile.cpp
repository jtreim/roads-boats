#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/Transporter.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex_point.h>
#include <tiles/components/River.h>
#include <utils/id_utils.h>

using namespace tile;

Tile::Tile(const Terrain t) : m_p_id(utils::gen_uuid()), m_p_terrain(t)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_neighbors[i].reset();
    m_p_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }
  m_p_areas.push_back(Area(ALL_BORDERS));
}

Tile::Tile(const hex_point hp, const Terrain t)
    : m_p_id(utils::gen_uuid()), m_p_terrain(t), m_p_hex_point(hp)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_neighbors[i].reset();
    m_p_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }
  m_p_areas.push_back(Area(ALL_BORDERS));
}

Tile::Tile(const hex_point hp, const std::vector<River> rivers, const Terrain t)
    : m_p_id(utils::gen_uuid()), m_p_terrain(t), m_p_hex_point(hp),
      m_p_rivers(rivers)
{
  split_by_rivers();
}

Tile::Tile(const Tile &other)
    : m_p_id(other.m_p_id), m_p_terrain(other.m_p_terrain),
      m_p_hex_point(other.m_p_hex_point), m_p_neighbors(other.m_p_neighbors),
      m_p_rivers(other.m_p_rivers), m_p_areas(other.m_p_areas)
{
  for (int i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_walls[i] = other.m_p_walls[i];
  }
}

Tile::Tile() : m_p_id(utils::gen_uuid()), m_p_terrain(Terrain::desert)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_neighbors[i].reset();
    m_p_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }
  m_p_areas.push_back(Area(ALL_BORDERS));
}

Tile::~Tile()
{
  m_p_hex_point = hex_point(0, 0);
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_neighbors[i].reset();
    m_p_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }
  m_p_rivers.clear();
  m_p_areas.clear();
}

bool Tile::operator==(Tile &other) const { return m_p_id == other.m_p_id; }
bool Tile::operator!=(Tile &other) const { return m_p_id != other.m_p_id; }

std::shared_ptr<Area> Tile::get_area(const Border b) const
{
  std::shared_ptr<Area> retval;
  for (auto a : m_p_areas)
  {
    if (a.has_border(b))
    {
      retval = std::make_shared<Area>(a);
      break;
    }
  }
  return retval;
}

std::shared_ptr<Tile> Tile::get_neighbor(Direction direction) const
{
  return m_p_neighbors[direction];
}

std::shared_ptr<Tile> *Tile::get_neighbors() { return m_p_neighbors; }

std::set<Direction> Tile::get_all_river_points() const
{
  std::set<Direction> retval;
  for (auto river : m_p_rivers)
  {
    retval.insert(river.get_points().begin(), river.get_points().end());
  }

  return retval;
}

bool Tile::is_neighboring_sea() const
{
  bool retval = false;
  for (auto neighbor : m_p_neighbors)
  {
    if ((neighbor) && (Terrain::sea == neighbor->m_p_terrain))
    {
      retval = true;
      break;
    }
  }
  return retval;
}

bool Tile::is_shore() const
{
  return ((!m_p_rivers.empty() || is_neighboring_sea()) &&
          (Terrain::sea != m_p_terrain));
}

common::Error Tile::add_neighbor(std::shared_ptr<Tile> neighbor,
                                 Direction direction)
{
  common::Error err = can_add_neighbor(neighbor, direction);
  if (common::ERR_NONE == err)
  {
    m_p_neighbors[direction] = neighbor;
  }
  return err;
}

common::Error Tile::remove_neighbor(Direction direction)
{
  common::Error err = common::ERR_NONE;
  if (!is_valid(direction))
  {
    err = common::ERR_INVALID;
  }
  else if (!m_p_neighbors[direction])
  {
    err = common::ERR_FAIL;
  }
  else
  {
    m_p_neighbors[direction].reset();
    m_p_neighbors[direction] = 0;
  }

  return err;
}

std::shared_ptr<building::Building> Tile::get_building() const
{
  std::shared_ptr<building::Building> retval;
  for (auto area : m_p_areas)
  {
    if (area.get_building())
    {
      retval = area.get_building();
      break;
    }
  }
  return retval;
}

std::map<std::string, std::pair<portable::Resource, uint8_t>>
Tile::get_all_resources() const
{
  // TODO: query all areas, return all resources found in each area (if any)
  return std::map<std::string, std::pair<portable::Resource, uint8_t>>();
}

std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
Tile::get_all_transporters() const
{
  // TODO: query all areas and rivers, return all transporters found (if any)
  return std::map<player::Color,
                  std::vector<std::shared_ptr<portable::Transporter>>>();
}

std::vector<std::shared_ptr<portable::Transporter>>
Tile::get_all_player_transporters(player::Color color) const
{
  // TODO: query all areas/rivers, return all transporters for input player.
  std::vector<std::shared_ptr<portable::Transporter>> transporters;
  return transporters;
}

bool Tile::has_river_point(const Direction direction) const
{
  bool retval = false;
  for (auto river : m_p_rivers)
  {
    if (river.has_point(direction))
    {
      retval = true;
      break;
    }
  }
  return retval;
}

common::Error Tile::can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                     Direction direction)
{
  common::Error err = common::ERR_NONE;
  // Check that input is valid, that we don't already have a neighbor in
  // that direction, and that we're not already neighbors with the new neighbor.
  if ((nullptr == neighbor) || (!is_valid(direction)))
  {
    err = common::ERR_INVALID;
  }
  else if ((m_p_neighbors[direction]) || (*this == *neighbor))
  {
    err = common::ERR_FAIL;
  }
  else
  {
    for (int d = 0; d < MAX_DIRECTIONS; d++)
    {
      if ((m_p_neighbors[d]) && (*(m_p_neighbors[d]) == *neighbor))
      {
        err = common::ERR_FAIL;
        break;
      }
    }
  }

  // Check for river points on the borders. Each tile's border should match
  // (adding a Sea tile neighbor should skip this step).
  if ((common::ERR_NONE == err) && (Terrain::sea != neighbor->m_p_terrain))
  {
    // From the neighbor's perspective, the matching side is the opposite
    // direction of the side we're checking. This tile and the neighbor we're
    // testing should match having a river/no river on the adjoining sides.
    Direction opposite = !direction;
    if (has_river_point(direction) != neighbor->has_river_point(opposite))
    {
      err = common::ERR_FAIL;
    }
  }
  return err;
}

common::Error Tile::clear_neighbors()
{
  common::Error err = common::ERR_NONE;
  for (int d = 0; d < MAX_DIRECTIONS; d++)
  {
    if (m_p_neighbors[d])
    {
      m_p_neighbors[d].reset();
      m_p_neighbors[d] = 0;
    }

    // If we somehow fail to remove the neighbor, report error.
    if (m_p_neighbors[d])
    {
      err = common::ERR_FAIL;
      break;
    }
  }
  return err;
}

void Tile::rotate(int8_t rotations)
{
  // Making 0 rotations doesn't do anything...
  if (0 != rotations)
  {
    bool is_clockwise = (0 < rotations);
    rotations = abs(rotations) % MAX_DIRECTIONS;
    rotations = (is_clockwise ? rotations : (MAX_DIRECTIONS - rotations));

    for (auto river : m_p_rivers)
    {
      river.rotate(rotations);
    }

    std::shared_ptr<Tile> tmp_t[MAX_DIRECTIONS];
    std::copy(std::begin(m_p_neighbors), std::end(m_p_neighbors),
              std::begin(tmp_t));
    for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
    {
      int idx = (rotations + i) % MAX_DIRECTIONS;
      m_p_neighbors[idx] = tmp_t[i];
    }
  }
}

// std::ostream &operator<<(std::ostream &os, const Tile &tile)
// {
//   os << "<Tile::id=" << tile.get_id()
//      << ", hex=" << tile.get_hex_point()
//      << ", terrain=" << to_string(tile.get_terrain());
//      << ", rivers=[" << tile.get_rivers()[0];
//   for (auto river : tile.get_rivers())
//   {
//     os <<
//   }
// }

nlohmann::json Tile::to_json() const
{
  nlohmann::json result;
  result["id"] = uuids::to_string(m_p_id);

  result["hex_coord"] = m_p_hex_point.to_json();

  // Add immediate neighbor's IDs
  std::vector<std::string> neighbor_ids;
  for (auto neighbor : m_p_neighbors)
  {
    if (nullptr == neighbor)
    {
      neighbor_ids.push_back("empty");
    }
    else
    {
      neighbor_ids.push_back(uuids::to_string(neighbor->get_id()));
    }
  }
  result["neighbors"] = neighbor_ids;

  for (auto river : m_p_rivers)
  {
    result["rivers"].push_back(river.to_json());
  }

  for (auto area : m_p_areas)
  {
    result["areas"].push_back(area.to_json());
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    std::pair<player::Color, uint8_t> wall = m_p_walls[i];
    Direction d = static_cast<Direction>(1 + i);
    result["walls"][to_string(d)]["color"] = to_string(wall.first);
    result["walls"][to_string(d)]["thickness"] = wall.second;
  }

  return result;
}

void Tile::split_by_rivers()
{
  std::set<std::set<Border>> remaining_borders;
  remaining_borders.insert(ALL_BORDERS);
  for (River r : m_p_rivers)
  {
    std::vector<std::set<Border>> add;
    std::vector<std::set<Border>> remove;

    for (auto b : remaining_borders)
    {
      // Check to see if this river affects this set of borders.
      if (r.splits_borders(b))
      {
        // It does, so we'll replace the old value with the new sets
        remove.push_back(b);
        std::vector<std::set<Border>> new_borders = r.get_area_borders(b);
        add.insert(add.end(), new_borders.begin(), new_borders.end());
      }
    }

    // Remove all border sets to be updated
    for (auto rem : remove)
    {
      remaining_borders.erase(rem);
    }
    // Add updated border sets
    for (auto a : add)
    {
      remaining_borders.insert(a);
    }
  }

  // We've now defined each area's borders; make 'em.
  for (auto borders : remaining_borders)
  {
    m_p_areas.push_back(Area(borders));
  }
}

// TODO: implement from_json