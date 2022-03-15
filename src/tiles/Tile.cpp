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
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>
#include <utils/id_utils.h>

namespace tile
{

Tile::Tile(const Terrain t)
    : m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(false)
{
  init();
}

Tile::Tile(const Hex hex, const Terrain t)
    : m_p_hex(hex), m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(true)
{
  init();
}

Tile::Tile(const std::set<Direction> river_points, const Terrain t)
    : m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(false)
{
  m_p_rivers.push_back(std::make_shared<River>(river_points));
  init();
}

Tile::Tile(const Hex hex, std::set<Direction> river_points, const Terrain t)
    : m_p_hex(hex), m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(true)
{
  m_p_rivers.push_back(std::make_shared<River>(river_points));
  init();
}

Tile::Tile(const std::vector<std::set<Direction>> river_points, const Terrain t)
    : m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(false)
{
  for (auto rp : river_points)
  {
    m_p_rivers.push_back(std::make_shared<River>(rp));
  }
  init();
}

Tile::Tile(const Hex hex, const std::vector<std::set<Direction>> river_points,
           const Terrain t)
    : m_p_hex(hex), m_p_terrain(t), m_p_rot_locked(false), m_p_hex_set(true)
{
  for (auto rp : river_points)
  {
    m_p_rivers.push_back(std::make_shared<River>(rp));
  }
  init();
}

Tile::Tile(const Tile &other)
    : m_p_hex(other.m_p_hex), m_p_terrain(other.m_p_terrain),
      m_p_rivers(other.m_p_rivers), m_p_areas(other.m_p_areas),
      m_p_rot_locked(other.m_p_rot_locked), m_p_hex_set(other.m_p_hex_set)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_walls[i] = other.m_p_walls[i];
    m_p_neighbors[i] = other.m_p_neighbors[i];
  }
}

Tile::~Tile()
{
  reset();
  m_p_rivers.clear();
  m_p_areas.clear();
}

void Tile::init()
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }

  if (Terrain::sea == m_p_terrain)
  {
    // Don't allow sea tiles to have rivers. Sea tiles always have just one
    // area, which covers the entire tile.
    m_p_rivers.clear();
    m_p_areas.push_back(std::make_shared<Area>(ALL_BORDERS));
    return;
  }

  split_by_rivers();
}

void Tile::reset()
{
  m_p_hex = Hex();
  for (auto area : m_p_areas)
  {
    area->reset();
  }
  for (auto river : m_p_rivers)
  {
    river->reset();
  }
  clear_neighbors();
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_walls[i] = std::make_pair<player::Color, uint8_t>(player::neutral, 0);
  }
  m_p_rot_locked = false;
  m_p_hex_set = false;
}

Tile Tile::operator=(const Tile &other)
{
  m_p_hex = other.m_p_hex;
  m_p_terrain = other.m_p_terrain;
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_p_neighbors[i] = other.m_p_neighbors[i];
    m_p_walls[i] = other.m_p_walls[i];
  }

  m_p_rivers.clear();
  for (auto river : other.m_p_rivers)
  {
    m_p_rivers.push_back(river);
  }

  m_p_areas.clear();
  for (auto area : other.m_p_areas)
  {
    m_p_areas.push_back(area);
  }

  m_p_rot_locked = other.m_p_rot_locked;
  m_p_hex_set = other.m_p_hex_set;
  return (*this);
}

bool Tile::operator==(Tile &other) const
{
  if ((m_p_terrain != other.m_p_terrain) || (m_p_rivers != other.m_p_rivers) ||
      (m_p_areas != other.m_p_areas) ||
      (m_p_rot_locked != other.m_p_rot_locked))
  {
    return false;
  }

  // If both tiles have set their hex points, they should agree.
  if ((m_p_hex_set) && (other.m_p_hex_set) && (m_p_hex != other.m_p_hex))
  {
    return false;
  }

  for (Direction d : ALL_DIRECTIONS)
  {
    if (m_p_neighbors[d] != other.m_p_neighbors[d])
    {
      return false;
    }
    if (m_p_walls[d] != other.m_p_walls[d])
    {
      return false;
    }
  }
  return true;
}
bool Tile::operator!=(Tile &other) const { return !(*this == other); }

std::shared_ptr<River> Tile::get_river(const Direction d)
{
  std::shared_ptr<River> retval;
  for (size_t i = 0; i < m_p_rivers.size(); i++)
  {
    if (m_p_rivers.at(i)->has_point(d))
    {
      retval = m_p_rivers.at(i);
      break;
    }
  }
  return retval;
}

std::set<Direction> Tile::get_river_points() const
{
  std::set<Direction> retval;
  for (auto river : m_p_rivers)
  {
    std::set<Direction> pts = river->get_points();
    retval.insert(pts.begin(), pts.end());
  }

  return retval;
}

std::set<Direction> Tile::get_river_points(const Direction d)
{
  std::shared_ptr<River> river = get_river(d);
  if (river)
  {
    return river->get_points();
  }
  return std::set<Direction>();
}

std::shared_ptr<Area> Tile::get_area(const Border b)
{
  std::shared_ptr<Area> retval;
  for (size_t i = 0; i < m_p_areas.size(); i++)
  {
    if (m_p_areas.at(i)->has_border(b))
    {
      retval = m_p_areas.at(i);
      break;
    }
  }
  return retval;
}

std::shared_ptr<Tile> Tile::get_neighbor(Direction direction)
{
  return m_p_neighbors[direction];
}

std::shared_ptr<Tile> *Tile::get_neighbors() { return m_p_neighbors; }

std::map<Direction, std::pair<player::Color, uint8_t>>
Tile::get_built_walls() const
{
  std::map<Direction, std::pair<player::Color, uint8_t>> retval;
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    if (m_p_walls[i].second > 0)
    {
      retval.at(static_cast<Direction>(i)) = m_p_walls[i];
    }
  }
  return retval;
}

bool Tile::is_neighboring_sea() const
{
  for (auto neighbor : m_p_neighbors)
  {
    if ((neighbor) && (Terrain::sea == neighbor->m_p_terrain))
    {
      return true;
    }
  }
  return false;
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
    // Set the new neighbor's hex coordinates to match what we expect
    m_p_neighbors[direction]->set_hex(m_p_hex.neighbor(direction));
    m_p_rot_locked = true;
  }
  return err;
}

common::Error Tile::remove_neighbor(Direction direction)
{
  if (!is_valid(direction))
  {
    return common::ERR_INVALID;
  }
  if (!m_p_neighbors[direction])
  {
    return common::ERR_FAIL;
  }

  m_p_neighbors[direction].reset();

  return common::ERR_NONE;
}

std::shared_ptr<building::Building> Tile::get_building() const
{
  std::shared_ptr<building::Building> retval;
  for (auto area : m_p_areas)
  {
    if (area->get_building())
    {
      return area->get_building();
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

bool Tile::has_road(const Border border)
{
  return get_area(border)->has_road(border);
}

bool Tile::has_river_point(const Direction direction)
{
  std::shared_ptr<River> r = get_river(direction);
  return ((r) && (r->has_point(direction)));
}

bool Tile::has_wall() const
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    if (m_p_walls[i].second != 0)
    {
      return true;
    }
  }
  return false;
}

common::Error Tile::can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                     Direction direction)
{
  // Check that input is valid, that we have a hex coordinate, that we don't
  // already have a neighbor in that direction, and that we're not already
  // neighbors with the new neighbor.
  if ((!neighbor) || (!is_valid(direction)))
  {
    return common::ERR_INVALID;
  }
  if ((!m_p_hex_set) || (m_p_neighbors[direction]) || (*this == *neighbor))
  {
    return common::ERR_FAIL;
  }
  for (uint8_t d = 0; d < MAX_DIRECTIONS; d++)
  {
    if ((m_p_neighbors[d]) && (*(m_p_neighbors[d]) == *neighbor))
    {
      return common::ERR_FAIL;
    }
  }
  // If the neighbor has its hex coordinates defined, it should agree with where
  // we're trying to add it.
  if ((neighbor->has_hex()) &&
      (neighbor->get_hex() != m_p_hex.neighbor(direction)))
  {
    return common::ERR_FAIL;
  }

  // Check for river points on the borders. Each tile's border should match
  // (Sea tiles should skip this step).
  if ((Terrain::sea == neighbor->m_p_terrain) ||
      (Terrain::sea == m_p_terrain) ||
      (has_river_point(direction) == neighbor->has_river_point(!direction)))
  {
    // From the neighbor's perspective, the matching side is the opposite
    // direction of the side we're checking. This tile and the neighbor we're
    // testing should match having a river/no river on the adjoining sides.
    return common::ERR_NONE;
  }

  return common::ERR_FAIL;
}

common::Error Tile::clear_neighbors()
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    if (m_p_neighbors[i])
    {
      Direction d = static_cast<Direction>(i);
      common::Error err = m_p_neighbors[i]->remove_neighbor(!d);
      if (err)
      {
        // TODO: log out error from neighbor removing us.
        return common::ERR_FAIL;
      }
      m_p_neighbors[i].reset();
    }

    // If we somehow fail to remove the neighbor, report error.
    if (m_p_neighbors[i])
    {
      return common::ERR_FAIL;
    }
  }
  return common::ERR_NONE;
}

bool Tile::can_rotate() const
{
  if ((m_p_rot_locked) || (m_p_hex_set))
  {
    return false;
  }
  for (auto neighbor : m_p_neighbors)
  {
    if (neighbor)
    {
      return false;
    }
  }
  for (auto area : m_p_areas)
  {
    if (!area->can_rotate())
    {
      return false;
    }
  }
  for (auto river : m_p_rivers)
  {
    if (!river->can_rotate())
    {
      return false;
    }
  }
  return true;
}

common::Error Tile::rotate(int rotations)
{
  common::Error err = common::ERR_UNKNOWN;

  if (can_rotate())
  {
    // Making 0 rotations doesn't do anything...
    err = common::ERR_NONE;
    if (0 != rotations)
    {
      bool is_clockwise = (0 < rotations);
      rotations = abs(rotations) % MAX_DIRECTIONS;
      rotations = (is_clockwise ? rotations : (MAX_DIRECTIONS - rotations));

      for (auto river : m_p_rivers)
      {
        if (common::ERR_NONE != river->rotate(rotations))
        {
          // TODO: Log out error. This should never happen; we checked the
          // rivers before rotating!
          err = common::ERR_UNKNOWN;
        }
      }
      for (auto area : m_p_areas)
      {
        if (common::ERR_NONE != area->rotate(rotations))
        {
          // TODO: Log out error. This should never happen; we checked the areas
          // before rotating!
          err = common::ERR_UNKNOWN;
        }
      }
    }
  }
  else
  {
    err = common::ERR_FAIL;
  }
  return err;
}

common::Error Tile::build_road(const Border border)
{
  common::Error err = common::ERR_FAIL;
  Direction d = direction_from_border(border);
  if ((nullptr != m_p_neighbors[d]) && (Terrain::sea != m_p_terrain) &&
      (Terrain::sea != m_p_neighbors[d]->m_p_terrain))
  {
    err = get_area(border)->build(border);
    if ((!err) && (!m_p_neighbors[d]->has_road(!border)))
    {
      err = m_p_neighbors[d]->build_road(!border);
    }
  }
  if (!err)
  {
    m_p_rot_locked = true;
  }
  return err;
}

common::Error
Tile::build_building(const std::shared_ptr<Area> &area,
                     const std::shared_ptr<building::Building> &bldg)
{
  common::Error err = common::ERR_FAIL;
  // Don't allow buildings before having a hex point
  if (!m_p_hex_set)
  {
    return err;
  }
  Border b = (*area->get_borders().begin());
  if ((area == get_area(b)) && (nullptr == get_building()))
  {
    err = get_area(b)->build(bldg);
  }
  if (!err)
  {
    m_p_rot_locked = true;
  }
  return err;
}

common::Error Tile::build_bridge(const Direction point)
{
  common::Error err = common::ERR_FAIL;
  // Don't allow bridges before getting a hex point
  if (!m_p_hex_set)
  {
    return err;
  }
  if (has_river_point(point))
  {
    err = get_river(point)->build(point);
  }
  if (!err)
  {
    m_p_rot_locked = true;
  }
  return err;
}

nlohmann::json Tile::to_json() const
{
  nlohmann::json result;

  // result["hex_coord"] = ;

  // Add immediate neighbor coordinates
  std::vector<std::string> neighbors;
  for (auto neighbor : m_p_neighbors)
  {
    if (nullptr == neighbor)
    {
      neighbors.push_back("empty");
    }
    else
    {
      neighbors.push_back(neighbor->get_hex().to_string());
    }
  }
  result["neighbors"] = neighbors;

  for (auto river : m_p_rivers)
  {
    result["rivers"].push_back(river->to_json());
  }

  for (auto area : m_p_areas)
  {
    result["areas"].push_back(area->to_json());
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    std::pair<player::Color, uint8_t> wall = m_p_walls[i];
    Direction d = static_cast<Direction>(i);
    result["walls"][to_string(d)]["color"] = to_string(wall.first);
    result["walls"][to_string(d)]["thickness"] = wall.second;
  }

  return result;
}

void Tile::split_by_rivers()
{
  std::set<std::set<Border>> remaining_borders;
  remaining_borders.insert(ALL_BORDERS);
  for (auto r : m_p_rivers)
  {
    std::vector<std::set<Border>> add;
    std::vector<std::set<Border>> remove;

    for (auto b : remaining_borders)
    {
      // Check to see if this river affects this set of borders.
      if (r->splits_borders(b))
      {
        // It does, so we'll replace the old value with the new sets
        remove.push_back(b);
        std::vector<std::set<Border>> new_borders = r->get_area_borders(b);
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
    m_p_areas.push_back(std::make_shared<Area>(borders));
  }
}

std::ostream &operator<<(std::ostream &os, const tile::Tile &tile)
{
  os << "<Tile::hex=" << tile.get_hex()
     << ", terrain=" << tile::to_string(tile.get_terrain())
     << ", rot_locked=" << (tile.is_rot_locked() ? "true" : "false")
     << ", rivers=" << (tile.get_rivers().size())
     << ", areas=" << (tile.get_areas().size());
  if (tile.has_wall())
  {
    os << ", walls={";
    auto walls = tile.get_built_walls();
    for (std::map<tile::Direction, std::pair<player::Color, uint8_t>>::iterator
             it = walls.begin();
         it != walls.end(); ++it)
    {
      os << it->first << ":{" << player::to_string(it->second.first) << ", "
         << it->second.second << "}";
    }
    os << "}";
  }
  os << ">";
  return os;
}
} // namespace tile
  // TODO: implement from_json