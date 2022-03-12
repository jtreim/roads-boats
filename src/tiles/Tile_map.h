#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/Tile.h>
#include <tiles/components/Hex.h>

/// This map relies heavily on the q, r, s (cube/axial) coordinate system for
/// hexagons. Each tile will have a coordinate tuple associated with its
/// location in the map. For more information, visit
/// https://www.redblobgames.com/grids/hexagons.

namespace tile
{
class Tile_map
{
public:
  Tile_map();
  Tile_map(const Tile_map &other);
  ~Tile_map();

  inline void reset()
  {
    m_p_map.clear();
    m_p_locked = false;
  }

  Tile_map operator=(const Tile_map &other);

  /// Retrieves the Tile at the given coordinates on the map
  /// @param[in] q
  /// @param[in] r
  /// @param[in] s
  /// @param[out] tile Tile found at given coordinates. Null on error.
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_UNKNOWN on any other errors
  common::Error get_tile(const int q, const int r, const int s,
                         std::shared_ptr<Tile> &tile)
  {
    if (s != (-q - r))
    {
      return common::ERR_INVALID;
    }
    Hex coord(q, r);
    return get_tile(coord, tile);
  }

  /// Retrieves the Tile at the given coordinates on the map
  /// @param[in] q
  /// @param[in] r
  /// @param[out] tile Tile found at given coordinates. Null on error.
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_UNKNOWN on any other errors
  common::Error get_tile(const int q, const int r, std::shared_ptr<Tile> &tile)
  {
    Hex coord(q, r);
    return get_tile(coord, tile);
  }

  /// Retrieves the Tile at the given coordinates on the map
  /// @param[in] point Map coordinates
  /// @param[out] tile Tile found at given coordinates. Null on error.
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_UNKNOWN on any other errors
  common::Error get_tile(const Hex coord, std::shared_ptr<Tile> &tile);

  /// Adds the tile to the map at the input coordinates.
  /// @param[in] q
  /// @param[in] r
  /// @param[in] tile Tile to be added
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to add based on tile layout
  ///   - ERR_UNKNOWN on any other errors
  common::Error insert(const int q, const int r,
                       const std::shared_ptr<Tile> &tile)
  {
    Hex coord(q, r);
    return insert(coord, tile);
  }
  /// Adds the tile to the map at the input coordinates.
  /// @param[in] q
  /// @param[in] r
  /// @param[in] s
  /// @param[in] tile Tile to be added
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to add based on tile layout
  ///   - ERR_UNKNOWN on any other errors
  common::Error insert(const int q, const int r, const int s,
                       const std::shared_ptr<Tile> &tile)
  {
    if (s != (-q - r))
    {
      return common::ERR_INVALID;
    }
    Hex coord(q, r);
    return insert(coord, tile);
  }
  /// Adds the tile to the map at the input coordinates.
  /// @param[in] coord
  /// @param[in] tile Tile to be added
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to add based on tile layout
  ///   - ERR_UNKNOWN on any other errors
  common::Error insert(const Hex coord, const std::shared_ptr<Tile> &tile);

  /// Removes the tile from the map at the input coordinates.
  /// @param[in] q
  /// @param[in] r
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to remove
  ///   - ERR_UNKNOWN on any other errors
  common::Error remove(const int q, const int r)
  {
    Hex coord(q, r);
    return remove(coord);
  }
  /// Removes the tile from the map at the input coordinates.
  /// @param[in] q
  /// @param[in] r
  /// @param[in] s
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to remove
  ///   - ERR_UNKNOWN on any other errors
  common::Error remove(const int q, const int r, const int s)
  {
    if (s != (-q - r))
    {
      return common::ERR_INVALID;
    }
    Hex coord(q, r);
    return remove(coord);
  }
  /// Removes the tile from the map at the input coordinates.
  /// @param[in] coord
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_FAIL on failure to remove
  common::Error remove(const Hex coord);

  inline bool empty() const { return m_p_map.empty(); }
  inline bool is_locked() const { return m_p_locked; }
  inline void set_lock(const bool lock_status) { m_p_locked = lock_status; }
  inline size_t size() const { return m_p_map.size(); }

  // Checks that the current map is a valid map, as in no rivers run to a
  // nonexistent tile.
  bool is_valid() const;

  nlohmann::json to_json() const;

protected:
private:
  std::map<Hex, std::shared_ptr<Tile>> m_p_map;

  // Flag denoting whether tiles can still be added/removed from the map.
  bool m_p_locked;
};
} // namespace tile

#endif