#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/Tile.h>
#include <tiles/Tile_map.h>

namespace tile
{
Tile_map::Tile_map() : m_p_locked(false) {}

Tile_map::Tile_map(const Tile_map &other)
    : m_p_map(m_p_map), m_p_locked(other.m_p_locked)
{
}

Tile_map::~Tile_map() { reset(); }

Tile_map Tile_map::operator=(const Tile_map &other)
{
  m_p_locked = other.m_p_locked;
  m_p_map.clear();
  m_p_map = other.m_p_map;
  return (*this);
}

common::Error Tile_map::get_tile(const Hex coord, std::shared_ptr<Tile> &tile)
{
  tile.reset();
  if (m_p_map.contains(coord))
  {
    tile = m_p_map.at(coord);
    return common::ERR_NONE;
  }

  return common::ERR_FAIL;
}

common::Error Tile_map::insert(const Hex coord,
                               const std::shared_ptr<Tile> &tile)
{
  if (!tile)
  {
    return common::ERR_INVALID;
  }

  if ((m_p_locked) || (m_p_map.contains(coord)))
  {
    return common::ERR_FAIL;
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    Hex other_coord = coord.neighbor(d);
    if (!m_p_map.contains(other_coord))
    {
      continue;
    }

    common::Error add_err = m_p_map.at(other_coord)->can_add_neighbor(tile, !d);
    if (add_err)
    {
      return common::ERR_FAIL;
    }
  }

  m_p_map.insert({coord, tile});
  m_p_map.at(coord)->set_hex(coord);

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    Hex other_coord = coord.neighbor(d);
    if (m_p_map.contains(other_coord))
    {
      std::shared_ptr<Tile> other = m_p_map.at(other_coord);
      common::Error self_err = tile->add_neighbor(other, d);
      common::Error other_err = other->add_neighbor(tile, !d);
      if ((self_err) || (other_err))
      {
        // TODO: log out errors from inserting what we thought was valid tile.
        // We should never get here!
        return common::ERR_UNKNOWN;
      }
    }
  }
  return common::ERR_NONE;
}

common::Error Tile_map::remove(const Hex coord)
{
  if ((m_p_locked) || (!m_p_map.contains(coord)))
  {
    return common::ERR_FAIL;
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    Hex other_coord = coord.neighbor(d);
    if (m_p_map.contains(other_coord))
    {
      std::shared_ptr<Tile> neighbor = m_p_map.at(other_coord);
      common::Error remove_err = neighbor->remove_neighbor(!d);
      if (remove_err)
      {
        // TODO: log out errors from removing what we thought was valid tile.
        // We should never get here!
        return common::ERR_UNKNOWN;
      }
    }
  }
  m_p_map.erase(coord);
  return common::ERR_NONE;
}

bool Tile_map::is_valid() const
{
  if (m_p_map.size() == 0)
  {
    return false;
  }
  // All rivers must either feed into an adjacent tile's river, or into a sea
  // tile.
  for (auto &item : m_p_map)
  {
    std::shared_ptr<Tile> tile = item.second;
    std::set<Direction> river_points = tile->get_river_points();
    for (auto rp : river_points)
    {
      if (nullptr == tile->get_neighbor(rp))
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace tile