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
      common::Error err = m_p_map.at(other_coord)->add_neighbor(tile, !d);
      if (err)
      {
        // TODO: log out error from inserting what we thought was valid tile.
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
  m_p_map.at(coord).reset();
  return common::ERR_NONE;
}

} // namespace tile