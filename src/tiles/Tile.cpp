#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <buildings/Building.h>
#include <players/Player.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>
#include <utils/id_utils.h>

namespace tile
{

Tile::Tile(const Terrain t)
    : m_terrain(t), m_rot_locked(false), m_hex_set(false),
      m_neighbors_are_current(true)
{
  init();
}

Tile::Tile(const Hex hex, const Terrain t)
    : m_hex(hex), m_terrain(t), m_rot_locked(false), m_hex_set(true),
      m_neighbors_are_current(true)
{
  init();
}

Tile::Tile(const std::set<Direction> river_points, const Terrain t)
    : m_terrain(t), m_rot_locked(false), m_hex_set(false),
      m_neighbors_are_current(true)
{
  m_rivers.push_back(std::make_shared<River>(river_points));
  init();
}

Tile::Tile(const Hex hex, std::set<Direction> river_points, const Terrain t)
    : m_hex(hex), m_terrain(t), m_rot_locked(false), m_hex_set(true),
      m_neighbors_are_current(true)
{
  m_rivers.push_back(std::make_shared<River>(river_points));
  init();
}

Tile::Tile(const std::vector<std::set<Direction>> river_points, const Terrain t)
    : m_terrain(t), m_rot_locked(false), m_hex_set(false),
      m_neighbors_are_current(true)
{
  for (auto rp : river_points)
  {
    m_rivers.push_back(std::make_shared<River>(rp));
  }
  init();
}

Tile::Tile(const Hex hex, const std::vector<std::set<Direction>> river_points,
           const Terrain t)
    : m_hex(hex), m_terrain(t), m_rot_locked(false), m_hex_set(true),
      m_neighbors_are_current(true)
{
  for (auto rp : river_points)
  {
    m_rivers.push_back(std::make_shared<River>(rp));
  }
  init();
}

Tile::Tile(const Tile &other)
    : m_hex(other.m_hex), m_terrain(other.m_terrain), m_rivers(other.m_rivers),
      m_areas(other.m_areas), m_rot_locked(other.m_rot_locked),
      m_hex_set(other.m_hex_set),
      m_neighbors_are_current(other.m_neighbors_are_current)
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_walls[i] = other.m_walls[i];
    m_neighbors[i] = other.m_neighbors[i];
  }
}

Tile::~Tile()
{
  reset();
  m_rivers.clear();
  m_areas.clear();
}

void Tile::init()
{
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_walls[i] =
        std::make_pair<player::Color, uint8_t>(player::Color::neutral, 0);
  }

  if (Terrain::sea == m_terrain)
  {
    // Don't allow sea tiles to have rivers. Sea tiles always have just one
    // area, which covers the entire tile.
    m_rivers.clear();
    m_areas.push_back(std::make_shared<Area>(ALL_BORDERS));
    return;
  }

  split_by_rivers();
}

void Tile::reset()
{
  m_hex = Hex();
  for (auto area : m_areas)
  {
    area->reset();
  }
  for (auto river : m_rivers)
  {
    river->reset();
  }
  clear_neighbors();
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_walls[i] = std::make_pair<player::Color, uint8_t>(player::neutral, 0);
  }
  m_rot_locked = false;
  m_hex_set = false;
  m_neighbors_are_current = true;
}

Tile Tile::operator=(const Tile &other)
{
  m_hex = other.m_hex;
  m_terrain = other.m_terrain;
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    m_neighbors[i] = other.m_neighbors[i];
    m_walls[i] = other.m_walls[i];
  }

  m_rivers = other.m_rivers;
  m_areas = other.m_areas;

  m_rot_locked = other.m_rot_locked;
  m_hex_set = other.m_hex_set;
  m_neighbors_are_current = other.m_neighbors_are_current;
  return (*this);
}

bool Tile::operator==(Tile &other) const
{
  if ((m_terrain != other.m_terrain) || (m_rivers != other.m_rivers) ||
      (m_areas != other.m_areas) || (m_rot_locked != other.m_rot_locked))
  {
    return false;
  }

  // If both tiles have set their hex points, they should agree.
  if ((m_hex_set) && (other.m_hex_set) && (m_hex != other.m_hex))
  {
    return false;
  }

  if ((!m_neighbors_are_current) || (!other.m_neighbors_are_current))
  {
    // A little more lax on checking neighbors; just check that both tiles say
    // they should have neighbors in the same direction
    for (Direction d : ALL_DIRECTIONS)
    {
      if (((nullptr == m_neighbors[d]) && (nullptr != other.m_neighbors[d])) ||
          ((nullptr == other.m_neighbors[d]) && (nullptr != m_neighbors[d])))
      {
        return false;
      }
      if (m_walls[d] != other.m_walls[d])
      {
        return false;
      }
    }
  }
  else
  {
    for (Direction d : ALL_DIRECTIONS)
    {
      if (m_neighbors[d] != other.m_neighbors[d])
      {
        return false;
      }
      if (m_walls[d] != other.m_walls[d])
      {
        return false;
      }
    }
  }
  return true;
}
bool Tile::operator==(Tile const &other) const
{
  if ((m_terrain != other.m_terrain) || (m_rivers != other.m_rivers) ||
      (m_areas != other.m_areas) || (m_rot_locked != other.m_rot_locked))
  {
    return false;
  }

  // If both tiles have set their hex points, they should agree.
  if ((m_hex_set) && (other.m_hex_set) && (m_hex != other.m_hex))
  {
    return false;
  }

  if ((!m_neighbors_are_current) || (!other.m_neighbors_are_current))
  {
    // A little more lax on checking neighbors; just check that both tiles say
    // they should have neighbors in the same direction
    for (Direction d : ALL_DIRECTIONS)
    {
      if (((nullptr == m_neighbors[d]) && (nullptr != other.m_neighbors[d])) ||
          ((nullptr == other.m_neighbors[d]) && (nullptr != m_neighbors[d])))
      {
        return false;
      }
      if (m_walls[d] != other.m_walls[d])
      {
        return false;
      }
    }
  }
  else
  {
    for (Direction d : ALL_DIRECTIONS)
    {
      if (m_neighbors[d] != other.m_neighbors[d])
      {
        return false;
      }
      if (m_walls[d] != other.m_walls[d])
      {
        return false;
      }
    }
  }
  return true;
}
bool Tile::operator!=(Tile &other) const { return !(*this == other); }
bool Tile::operator!=(Tile const &other) const { return !(*this == other); }

std::shared_ptr<River> Tile::get_river(const Direction d)
{
  std::shared_ptr<River> retval;
  for (size_t i = 0; i < m_rivers.size(); i++)
  {
    if (m_rivers.at(i)->has_point(d))
    {
      retval = m_rivers.at(i);
      break;
    }
  }
  return retval;
}

std::set<Direction> Tile::get_river_points() const
{
  std::set<Direction> retval;
  for (auto river : m_rivers)
  {
    std::set<Direction> pts = river->get_points();
    retval.insert(pts.begin(), pts.end());
  }

  return retval;
}

std::set<Direction> Tile::get_bridges() const
{
  std::set<Direction> retval;
  for (auto river : m_rivers)
  {
    std::set<Direction> bridges = river->get_bridges();
    retval.insert(bridges.begin(), bridges.end());
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
  for (size_t i = 0; i < m_areas.size(); i++)
  {
    if (m_areas.at(i)->has_border(b))
    {
      retval = m_areas.at(i);
      break;
    }
  }
  return retval;
}

std::shared_ptr<Tile> Tile::get_neighbor(Direction direction)
{
  return m_neighbors[direction];
}

std::shared_ptr<Tile> *Tile::get_neighbors() { return m_neighbors; }

std::map<Direction, std::pair<player::Color, uint8_t>>
Tile::get_built_walls() const
{
  std::map<Direction, std::pair<player::Color, uint8_t>> retval;
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    if (m_walls[i].second > 0)
    {
      retval.at(static_cast<Direction>(i)) = m_walls[i];
    }
  }
  return retval;
}

bool Tile::is_neighboring_sea() const
{
  for (auto neighbor : m_neighbors)
  {
    if ((neighbor) && (Terrain::sea == neighbor->m_terrain))
    {
      return true;
    }
  }
  return false;
}

bool Tile::is_shore() const
{
  return ((!m_rivers.empty() || is_neighboring_sea()) &&
          (Terrain::sea != m_terrain));
}

common::Error Tile::add_neighbor(std::shared_ptr<Tile> neighbor,
                                 Direction direction)
{
  common::Error err = can_add_neighbor(neighbor, direction);
  if (common::ERR_NONE == err)
  {
    m_neighbors[direction] = neighbor;
    // Set the new neighbor's hex coordinates to match what we expect
    m_neighbors[direction]->set_hex(m_hex.neighbor(direction));
    m_rot_locked = true;
  }
  return err;
}

common::Error Tile::remove_neighbor(Direction direction)
{
  if (!is_valid(direction))
  {
    return common::ERR_INVALID;
  }
  if (!m_neighbors[direction])
  {
    return common::ERR_FAIL;
  }

  m_neighbors[direction].reset();

  return common::ERR_NONE;
}

std::shared_ptr<building::Building> Tile::get_building() const
{
  std::shared_ptr<building::Building> retval;
  for (auto area : m_areas)
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
    if (m_walls[i].second != 0)
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
  if ((!m_hex_set) || (m_neighbors[direction]) || (*this == *neighbor))
  {
    return common::ERR_FAIL;
  }
  for (uint8_t d = 0; d < MAX_DIRECTIONS; d++)
  {
    if ((m_neighbors[d]) && (*(m_neighbors[d]) == *neighbor))
    {
      return common::ERR_FAIL;
    }
  }
  // If the neighbor has its hex coordinates defined, it should agree with where
  // we're trying to add it.
  if ((neighbor->has_hex()) &&
      (neighbor->get_hex() != m_hex.neighbor(direction)))
  {
    return common::ERR_FAIL;
  }

  // Check for river points on the borders. Each tile's border should match
  // (Sea tiles should skip this step).
  if ((Terrain::sea == neighbor->m_terrain) || (Terrain::sea == m_terrain) ||
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
    if (m_neighbors[i])
    {
      Direction d = static_cast<Direction>(i);
      common::Error err = m_neighbors[i]->remove_neighbor(!d);
      if (err)
      {
        // TODO: log out error from neighbor removing us.
        return common::ERR_FAIL;
      }
      m_neighbors[i].reset();
    }

    // If we somehow fail to remove the neighbor, report error.
    if (m_neighbors[i])
    {
      return common::ERR_FAIL;
    }
  }
  m_neighbors_are_current = true;
  return common::ERR_NONE;
}

bool Tile::can_rotate() const
{
  if ((m_rot_locked) || (m_hex_set))
  {
    return false;
  }
  for (auto neighbor : m_neighbors)
  {
    if (neighbor)
    {
      return false;
    }
  }
  for (auto area : m_areas)
  {
    if (!area->can_rotate())
    {
      return false;
    }
  }
  for (auto river : m_rivers)
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

      for (auto river : m_rivers)
      {
        if (common::ERR_NONE != river->rotate(rotations))
        {
          // TODO: Log out error. This should never happen; we checked the
          // rivers before rotating!
          err = common::ERR_UNKNOWN;
        }
      }
      for (auto area : m_areas)
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
  if ((m_neighbors_are_current) &&
      ((nullptr != m_neighbors[d]) && (Terrain::sea != m_terrain) &&
       (Terrain::sea != m_neighbors[d]->m_terrain)))
  {
    err = get_area(border)->build(border);
    if ((!err) && (!m_neighbors[d]->has_road(!border)))
    {
      err = m_neighbors[d]->build_road(!border);
    }
  }
  if (!err)
  {
    m_rot_locked = true;
  }
  return err;
}

common::Error
Tile::build_building(const std::shared_ptr<Area> &area,
                     const std::shared_ptr<building::Building> &bldg)
{
  common::Error err = common::ERR_FAIL;
  // Don't allow buildings before having a hex point or all the neighbors' data
  if ((!m_hex_set) || (!m_neighbors_are_current))
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
    m_rot_locked = true;
  }
  return err;
}

common::Error Tile::build_bridge(const Direction point)
{
  common::Error err = common::ERR_FAIL;
  // Don't allow bridges before getting a hex point or all our neighbors' data
  if ((!m_hex_set) || (!m_neighbors_are_current))
  {
    return err;
  }
  if (has_river_point(point))
  {
    err = get_river(point)->build(point);
  }
  if (!err)
  {
    m_rot_locked = true;
  }
  return err;
}

bool Tile::can_build_wall(const Direction side, const player::Color color,
                          const uint8_t thickness)
{
  // If we don't have a neighbor to that side, the color of the wall is neutral,
  // or the thickness is 0, return false
  if ((!m_neighbors[side]) || (player::Color::neutral == color))
  {
    return false;
  }
  // If the current wall isn't neutral, we can only add the same color wall
  if ((player::Color::neutral != m_walls[side].first) &&
      (color != m_walls[side].first))
  {
    return false;
  }
  // If there was a wall that has since been destroyed, return false
  if ((player::Color::neutral == m_walls[side].first) &&
      (0 < m_walls[side].second))
  {
    return false;
  }
  return true;
}

common::Error Tile::build_wall(const Direction side, const player::Color color,
                               const uint8_t thickness)
{
  if ((!is_valid(side)) || (!player::is_valid(color)) || (0 == thickness))
  {
    return common::ERR_INVALID;
  }
  if (!can_build_wall(side, color, thickness))
  {
    return common::ERR_FAIL;
  }
  m_walls[side].first = color;
  m_walls[side].second += thickness;
  return common::ERR_NONE;
}

void Tile::split_by_rivers()
{
  std::set<std::set<Border>> remaining_borders;
  remaining_borders.insert(ALL_BORDERS);
  for (auto r : m_rivers)
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
    m_areas.push_back(std::make_shared<Area>(borders));
  }
}

std::ostream &operator<<(std::ostream &os, const tile::Tile &tile)
{
  os << "<Tile::hex=" << tile.get_hex()
     << ", hex_set=" << (tile.m_hex_set ? "true" : "false")
     << ", terrain=" << tile::to_string(tile.get_terrain())
     << ", rot_locked=" << (tile.is_rot_locked() ? "true" : "false")
     << ", rivers=" << (tile.get_rivers().size())
     << ", areas=" << (tile.get_areas().size()) << ", neighbors_are_current="
     << (tile.m_neighbors_are_current ? "true" : "false");
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

void Tile::load_rivers_json(const nlohmann::json &j)
{
  m_rivers.clear();
  if ((Terrain::sea == m_terrain) && (j.get<std::vector<River>>().size() != 0))
  {
    reset();
    throw nlohmann::json::type_error::create(501, "Rivers listed for sea tile!",
                                             j);
  }
  for (auto river : j.get<std::vector<River>>())
  {
    for (auto point : river.get_points())
    {
      if (has_river_point(point))
      {
        reset();
        std::stringstream msg;
        msg << "Duplicate river point in tile JSON! point=" << point;
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
    }
    m_rivers.push_back(std::make_shared<River>(river));
  }
}

void Tile::load_neighbors_json(const nlohmann::json &j)
{
  // The best the tile can do for loading its neighbors is to create blank
  // tiles, and indicate it needs further data from the tile map.
  if (j.get<std::vector<Direction>>().size() > 0)
  {
    m_neighbors_are_current = false;
  }
  for (auto d : j.get<std::vector<Direction>>())
  {
    Hex h = m_hex.neighbor(d);
    std::shared_ptr<Tile> neighbor = std::make_shared<Tile>(h);
    if (has_river_point(d))
    {
      std::set<Direction> rp;
      rp.insert(!d);
      neighbor = std::make_shared<Tile>(h, rp);
    }
    common::Error err = add_neighbor(neighbor, d);
    if (common::ERR_NONE != err)
    {
      reset();
      std::stringstream msg;
      msg << "Invalid neighbor value!";
      throw nlohmann::json::type_error::create(501, msg.str(), j);
    }
  }
}

void Tile::load_areas_json(const nlohmann::json &j)
{
  m_areas.clear();
  split_by_rivers();
  // Validate that all areas are as expected based on the rivers/neighbors
  for (auto area : j.get<std::vector<Area>>())
  {
    std::set<Border> borders = area.get_borders();
    auto first_border = borders.begin();
    std::shared_ptr<Area> matching_area = get_area(*first_border);
    if ((*matching_area) != borders)
    {
      reset();
      throw nlohmann::json::type_error::create(
          501, "Invalid area borders specified in tile JSON!", j);
    }
    for (auto road : area.get_roads())
    {
      if (!matching_area->can_build_road(road))
      {
        reset();
        std::stringstream msg;
        msg << "Invalid area road specified in tile JSON! Road border=" << road;
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
    }
  }
  m_areas.clear();
  for (auto area : j.get<std::vector<Area>>())
  {
    m_areas.push_back(std::make_shared<Area>(area));
  }
}

void Tile::load_walls_json(const nlohmann::json &j)
{
  std::set<Direction> loaded_walls;
  for (auto wall : j.get<std::vector<nlohmann::json>>())
  {
    Direction d = wall.at("side").get<Direction>();
    player::Color color = wall.at("color").get<player::Color>();
    uint8_t thickness = wall.at("thickness").get<uint8_t>();
    if ((Direction::invalid_direction == d) || loaded_walls.contains(d))
    {
      reset();
      throw nlohmann::json::type_error::create(
          501, "Invalid side given for wall!", j);
    }
    if (player::Color::invalid == color)
    {
      reset();
      throw nlohmann::json::type_error::create(
          501, "Invalid player color given as wall color!", j);
    }
    loaded_walls.insert(d);
    if (player::Color::neutral != color)
    {
      common::Error err = build_wall(d, color, thickness);
      if (common::ERR_NONE != err)
      {
        reset();
        std::stringstream msg;
        msg << "Invalid wall specified! side=" << d << ", color=" << color
            << ", thickness=" << thickness;
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
    }
    else
    {
      m_walls[d].first = color;
      m_walls[d].second = thickness;
    }
  }
}

void to_json(nlohmann::json &j, const Tile &tile)
{
  nlohmann::json hex_json;
  to_json(hex_json, tile.m_hex);
  j["hex"] = hex_json;
  j["terrain"] = to_string(tile.m_terrain);
  // Add immediate neighbor coordinates
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    std::shared_ptr<Tile> neighbor = tile.m_neighbors[i];
    if (neighbor)
    {
      j["neighbors"].push_back(to_string(static_cast<Direction>(i)));
    }
  }

  for (auto river : tile.m_rivers)
  {
    nlohmann::json river_json;
    to_json(river_json, (*river));
    j["rivers"].push_back(river_json);
  }

  for (auto area : tile.m_areas)
  {
    nlohmann::json area_json;
    to_json(area_json, (*area));
    j["areas"].push_back(area_json);
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    std::pair<player::Color, uint8_t> wall = tile.m_walls[i];
    Direction d = static_cast<Direction>(i);
    nlohmann::json wall_json;
    wall_json["color"] = to_string(wall.first);
    wall_json["thickness"] = wall.second;
    wall_json["side"] = to_string(d);
    j["walls"].push_back(wall_json);
  }

  j["rot_locked"] = tile.m_rot_locked;
  j["hex_set"] = tile.m_hex_set;
  // We shouldn't save the "neighbors current" flag. The flag is
  // for loading from JSON, and indicates whether the tile has been fully
  // loaded.
}

void from_json(const nlohmann::json &j, Tile &tile)
{
  tile = Tile();
  tile.m_hex = j.at("hex").get<Hex>();
  tile.m_terrain = j.at("terrain").get<Terrain>();
  tile.m_hex_set = j.at("hex_set").get<bool>();
  tile.m_rot_locked = j.at("rot_locked").get<bool>();
  tile.load_rivers_json(j.at("rivers"));
  tile.load_neighbors_json(j.at("neighbors"));
  tile.load_areas_json(j.at("areas"));
  tile.load_walls_json(j.at("walls"));
}
} // namespace tile