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
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>
#include <utils/id_utils.h>

using namespace tile;

Tile::Tile() : id(utils::gen_uuid()) {}

Tile::Tile(std::set<Direction> p_river_points)
    : id(utils::gen_uuid()), m_p_river_points(p_river_points)
{
}

Tile::~Tile()
{
  for (uint8_t i = 0; i < m_max_directions; i++)
  {
    m_p_neighbors[i].reset();
  }
  m_p_river_points.clear();
  m_p_building.reset();
  m_p_resources.clear();
  m_p_transporters.clear();
}

bool Tile::operator==(Tile const &other) const { return id == other.get_id(); }
bool Tile::operator!=(Tile const &other) const { return id != other.get_id(); }

std::shared_ptr<Tile> Tile::get_neighbor(Direction direction) const
{
  return m_p_neighbors[direction];
}

std::shared_ptr<Tile> *Tile::get_neighbors() { return m_p_neighbors; }

bool Tile::river_has_point(Direction direction) const
{
  return (m_p_river_points.end() != m_p_river_points.find(direction));
}

std::set<Direction> Tile::get_river_points() const { return m_p_river_points; }

bool Tile::is_neighboring_sea() const
{
  bool retval = false;
  for (auto neighbor : m_p_neighbors)
  {
    if (neighbor->is_sea())
    {
      retval = true;
      break;
    }
  }
  return retval;
}

bool Tile::is_shore() const
{
  return (!m_p_river_points.empty() || is_neighboring_sea());
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
  if (!is_valid_direction(direction))
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
  return m_p_building;
}

std::map<std::string, std::pair<portable::Resource, uint8_t>>
Tile::get_resources() const
{
  return m_p_resources;
}

std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
Tile::get_transporters() const
{
  return m_p_transporters;
}

std::vector<std::shared_ptr<portable::Transporter>>
Tile::get_player_transporters(player::Color color) const
{
  std::vector<std::shared_ptr<portable::Transporter>> transporters;
  if (m_p_transporters.end() != m_p_transporters.find(color))
  {
    transporters = m_p_transporters.at(color);
  }
  return transporters;
}

Direction Tile::get_opposite_direction(Direction direction)
{
  return static_cast<Direction>((direction + m_max_directions / 2) %
                                m_max_directions);
}

common::Error Tile::can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                     Direction direction)
{
  common::Error err = common::ERR_NONE;
  // Check that input is valid, that we don't already have a neighbor in
  // that direction, and that we're not already neighbors with the new neighbor.
  if ((nullptr == neighbor) || (!is_valid_direction(direction)))
  {
    err = common::ERR_INVALID;
  }
  else if ((m_p_neighbors[direction]) || (*this == *neighbor))
  {
    err = common::ERR_FAIL;
  }
  else
  {
    for (int d = 0; d < m_max_directions; d++)
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
  if ((common::ERR_NONE == err) && (!neighbor->is_sea()))
  {
    bool contains_river = river_has_point(direction);

    // From the neighbor's perspective, the matching border is the opposite
    // direction of the border we're checking.
    Direction opposite = get_opposite_direction(direction);

    // Both this tile and the neighbor we're testing should match having a
    // river/no river on the adjoining sides.
    if (neighbor->river_has_point(opposite) != contains_river)
    {
      err = common::ERR_FAIL;
    }
  }
  return err;
}

common::Error Tile::clear_neighbors()
{
  common::Error err = common::ERR_NONE;
  for (int d = 0; d < m_max_directions; d++)
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

common::Error Tile::rotate(int8_t rotations)
{
  // Making 0 rotations doesn't do anything...
  if (0 != rotations)
  {
    bool is_clockwise = (0 < rotations);
    rotations = abs(rotations) % m_max_directions;
    rotations = (is_clockwise ? rotations : (m_max_directions - rotations));

    // Rotate all river points
    std::vector<Direction> tmp_rp(m_p_river_points.size());
    std::copy(m_p_river_points.begin(), m_p_river_points.end(), tmp_rp.begin());
    m_p_river_points.clear();
    for (auto direction : tmp_rp)
    {
      Direction new_dir =
          static_cast<Direction>((direction + rotations) % m_max_directions);
      m_p_river_points.insert(new_dir);
    }

    std::shared_ptr<Tile> tmp_t[m_max_directions];
    std::copy(std::begin(m_p_neighbors), std::end(m_p_neighbors),
              std::begin(tmp_t));
    for (int i = 0; i < m_max_directions; i++)
    {
      int idx = (rotations + i) % m_max_directions;
      m_p_neighbors[idx] = tmp_t[i];
    }
  }

  return common::ERR_NONE;
}

nlohmann::json Tile::to_json() const
{
  nlohmann::json result;
  result["id"] = uuids::to_string(id);

  // Add immediate neighbor's IDs
  std::vector<std::string> neighbor_ids;
  for (auto neighbor : m_p_neighbors)
  {
    if (nullptr == neighbor)
    {
      neighbor_ids.push_back(0);
    }
    else
    {
      neighbor_ids.push_back(uuids::to_string(neighbor->get_id()));
    }
  }
  result["neighbors"] = neighbor_ids;

  // Add river points
  std::vector<uint8_t> river_points;
  river_points.reserve(6);
  std::copy(m_p_river_points.begin(), m_p_river_points.end(),
            std::back_inserter(river_points));
  result["river_points"] = river_points;

  // TODO: Add building

  // TODO: Add resource counts

  // TODO: Add transporters

  return result;
}