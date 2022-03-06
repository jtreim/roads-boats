#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <common/Errors.h>
#include <tiles/Tile.h>
#include <tiles/components/Hex_point.h>

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
  ~Tile_map();

  /// Retrieves the Tile at the given coordinates on the map
  /// @param[in] q
  /// @param[in] r
  /// @param[in] s
  /// @param[out] tile Tile found at given coordinates. Null on error.
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_UNKNOWN on any other errors
  common::Error get_tile_at(const int8_t q, const int8_t r, const int8_t s,
                            std::shared_ptr<Tile> &tile)
  {
    if (s != (-q - r))
    {
      return common::ERR_INVALID;
    }
    hex_point coord(q, r);
    return get_tile_at(coord, tile);
  }

  /// Retrieves the Tile at the given coordinates on the map
  /// @param[in] point Map coordinates
  /// @param[out] tile Tile found at given coordinates. Null on error.
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_UNKNOWN on any other errors
  common::Error get_tile_at(const hex_point coord, std::shared_ptr<Tile> &tile);

  // /// Retrieves the Tile with the given ID on the map.
  // /// @param[in] id
  // /// @param[out] tile Tile found with given ID. Null on error.
  // /// @param[out] coord Coordinates of requested Tile. Null on error.
  // /// @return
  // ///   - ERR_NONE on success
  // ///   - ERR_INVALID on invalid input parameters
  // ///   - ERR_UNKNOWN on any other errors
  // common::Error get_tile_by_id(const uuids::uuid id,
  //                              std::shared_ptr<Tile> &tile,
  //                              std::shared_ptr<hex_point> &coord);

  // /// Retrieves the map coordinates for the Tile with the given ID
  // /// @param[in] id
  // /// @param[out] coord Coordinates for the tile. Null on error.
  // /// @return
  // ///   - ERR_NONE on success
  // ///   - ERR_INVALID on invalid input parameters
  // ///   - ERR_MISSING if tile not found in map
  // ///   - ERR_UNKNOWN on any other errors
  // common::Error get_tile_hex(const uuids::uuid id,
  //                            std::shared_ptr<hex_point> &coord);

  // /// Retrieves the map coordinates for the given Tile
  // /// @param[in] tile
  // /// @param[out] coord Coordinates for the tile. Null on error.
  // /// @return
  // ///   - ERR_NONE on success
  // ///   - ERR_INVALID on invalid input parameters
  // ///   - ERR_MISSING if tile not found in map
  // ///   - ERR_UNKNOWN on any other errors
  // common::Error get_tile_hex(const std::shared_ptr<Tile> tile,
  //                            std::shared_ptr<hex_point> &coord);

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
  common::Error add_tile(const int8_t q, const int8_t r, const int8_t s,
                         const std::shared_ptr<Tile> &tile)
  {
    if (s != (-q - r))
    {
      return common::ERR_INVALID;
    }
    hex_point coord(q, r);
    return add_tile(coord, tile);
  }

  /// Adds the tile to the map at the input coordinates.
  /// @param[in] coord
  /// @param[in] tile Tile to be added
  /// @return
  ///   - ERR_NONE on success
  ///   - ERR_INVALID on invalid input parameters
  ///   - ERR_FAIL on failure to add based on tile layout
  ///   - ERR_UNKNOWN on any other errors
  common::Error add_tile(const hex_point coord,
                         const std::shared_ptr<Tile> &tile);

  nlohmann::json to_json() const;

protected:
private:
};
} // namespace tile

#endif