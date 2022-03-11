#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/Tile.h>
#include <tiles/Tile_map.h>

namespace tile
{
Tile_map::Tile_map() : m_p_locked(false) {}

Tile_map::~Tile_map() { reset(); }

common::Error Tile_map::get_tile(const Hex coord, std::shared_ptr<Tile> &tile)
{
  if (m_p_map.at(coord))
  {
    tile = m_p_map.at(coord);
    return common::ERR_NONE;
  }

  return common::ERR_FAIL;
}

common::Error Tile_map::insert(const Hex coord,
                               const std::shared_ptr<Tile> &tile)
{
  if (m_p_locked)
  {
    return common::ERR_FAIL;
  }

  if ((m_p_map.at(coord)) || (!tile))
  {
    return common::ERR_INVALID;
  }

  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    Hex other_coord = coord.neighbor(d);
    if ((m_p_map.at(other_coord)) &&
        (!m_p_map.at(other_coord)->can_add_neighbor(tile, !d)))
    {
      return common::ERR_FAIL;
    }
  }

  m_p_map.at(coord) = tile;
  for (uint8_t i = 0; i < MAX_DIRECTIONS; i++)
  {
    Direction d = static_cast<Direction>(i);
    Hex other_coord = coord.neighbor(d);
    if (m_p_map.at(other_coord))
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
  if ((m_p_locked) || (!m_p_map.at(coord)))
  {
    return common::ERR_FAIL;
  }
  m_p_map.at(coord).reset();
  return common::ERR_NONE;
}

} // namespace tile