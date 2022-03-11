#ifndef TILE_H
#define TILE_H

#include <map>
#include <memory>
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
#include <tiles/components/Border.h>
#include <tiles/components/Hex.h>
#include <tiles/components/River.h>

namespace tile
{
enum Terrain
{
  invalid = -1,
  desert = 0,
  forest,
  mountain,
  plains,
  rock,
  sea
};
static const uint8_t MAX_TERRAIN_TYPES = 6;
static const std::string TERRAIN_NAMES[MAX_TERRAIN_TYPES]{
    "desert", "forest", "mountain", "plains", "rock", "sea"};
static bool is_valid(const Terrain t)
{
  return ((0 <= t) && (MAX_TERRAIN_TYPES > t));
}
static const std::string to_string(const Terrain t)
{
  if (is_valid(t))
  {
    return TERRAIN_NAMES[t];
  }
  return "unknown";
}

class Tile
{
public:
  Tile(const Terrain t = Terrain::desert);
  Tile(const Hex hex, const Terrain t = Terrain::desert);
  Tile(const std::set<Direction> river_points,
       const Terrain t = Terrain::desert);
  Tile(const Hex hex, std::set<Direction> river_points,
       const Terrain t = Terrain::desert);
  Tile(const std::vector<std::set<Direction>> river_points,
       const Terrain t = Terrain::desert);
  Tile(const Hex hex, const std::vector<std::set<Direction>> river_points,
       const Terrain t = Terrain::desert);
  Tile(const Tile &other);
  ~Tile();

  /// Initializes the Tile using the contructor's parameters, e.g.
  /// dividing the tile into areas using the input rivers.
  void init();
  /// Clears tile of all buildings/resources/neighbors.
  void reset();

  inline Terrain get_terrain() const { return m_p_terrain; }
  inline Hex get_hex() const { return m_p_hex; }
  inline bool has_hex() const { return m_p_hex_set; }
  inline void set_hex(const Hex hp)
  {
    m_p_hex = hp;
    m_p_hex_set = true;
  }
  inline void clear_hex() { m_p_hex_set = false; }
  inline bool is_rot_locked() const { return m_p_rot_locked; }

  /// Returns the river with a point at the input direction.
  /// @param[in] d Direction to check for a river point.
  /// @return A pointer to the river at the input direction. Null if no river
  /// found with that point.
  std::shared_ptr<River> get_river(const Direction d);
  inline std::vector<std::shared_ptr<River>> get_rivers() { return m_p_rivers; }
  inline std::vector<std::shared_ptr<River>> get_rivers() const
  {
    return m_p_rivers;
  }
  std::set<Direction> get_river_points() const;
  std::set<Direction> get_river_points(const Direction d);

  /// Returns the area that uses the input border.
  /// @param[in] b Border that area is a part of.
  /// @return A pointer to the area with the input border. Null on invalid
  /// border input.
  std::shared_ptr<Area> get_area(const Border b);
  /// Returns all areas of the tile.
  /// @return all of the tile's areas.
  inline std::vector<std::shared_ptr<Area>> get_areas() { return m_p_areas; }
  inline std::vector<std::shared_ptr<Area>> get_areas() const
  {
    return m_p_areas;
  }
  /// Returns the areas associated with the input tile side.
  /// @param[in] d side of the tile that areas are associated with.
  /// @return areas associated with the input side.
  std::vector<std::shared_ptr<Area>> get_areas(const Direction d);

  /// Gets the first adjacent tile in the input direction
  /// @param direction Side of the tile to check for a neighbor
  /// @return
  ///   - pointer to the first adjacent tile
  ///   - nullptr if no tile is in the given direction
  std::shared_ptr<Tile> get_neighbor(const Direction direction);
  std::shared_ptr<Tile> *get_neighbors();

  inline std::pair<player::Color, uint8_t> get_wall(const Direction d) const
  {
    if (is_valid(d))
    {
      return m_p_walls[d];
    }
    return std::make_pair<player::Color, uint8_t>(player::Color::invalid, 0);
  }

  std::map<Direction, std::pair<player::Color, uint8_t>>
  get_built_walls() const;

  /// Gets the building (if any) currently on this tile
  /// @return
  ///   - pointer to the structure built on this tile
  ///   - nullptr if no structure has been built here
  std::shared_ptr<building::Building> get_building() const;
  std::map<std::string, std::pair<portable::Resource, uint8_t>>
  get_all_resources() const;

  bool has_river_point(const Direction direction);
  bool has_road(const Border border);
  bool has_wall() const;
  bool is_shore() const;

  /// Checks whether neighbor can be placed at the direction relative to the
  /// tile.
  /// @param[in] neighbor  Tile to be placed
  /// @param[in] direction  Direction neighbor would be to the tile
  /// @return
  ///   - common::Error::ERR_INVALID if either param is an invalid format
  ///   (null, nonexistant direction).
  ///   - common::Error::ERR_FAIL if there's already a neighbor in the given
  ///   direction, the neighbor's river points doesn't allow being added
  ///   there, or if the new neighbor matches ourselves or a neighbor we
  ///   already have.
  ///   - common::Error::ERR_NONE on valid placement.
  common::Error can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                 Direction direction);

  /// Add a tile as a neighbor in the input direction.
  /// @param[in] neighbor The tile to be placed next to this one.
  /// @param[in] direction The direction the tile should be placed from first
  /// tile's perspective.
  /// @return
  ///   - common::Error::ERR_INVALID if either param is an invalid format
  ///   (null, nonexistant direction).
  ///   - common::Error::ERR_FAIL if there's already a neighbor in the given
  ///   direction, or the neighbor's river points doesn't allow being added
  ///   there.
  ///   - common::Error::ERR_NONE on success.
  common::Error add_neighbor(std::shared_ptr<Tile> neighbor,
                             Direction direction);

  /// Remove the neighbor from the input direction
  /// @param[in] neighbor The direction of the neighbor to be removed.
  /// @return
  ///   - common::Error::ERR_INVALID if the direction is invalid.
  ///   - common::Error::ERR_MISSING if there is no neighbor in the input
  ///   direction.
  ///   - common::Error::ERR_NONE on success.
  common::Error remove_neighbor(Direction direction);

  /// Removes all neighbors from the tile
  /// @return
  ///   - common::Error::ERR_NONE on success
  ///   - common::Error::ERR_FAIL on fail
  common::Error clear_neighbors();

  /// Check to see if tile can rotate. Don't allow if the tile has any
  /// neighbors; if at least one of its areas/rivers can't rotate; or if it is
  /// locked.
  /// @return true if tile isn't rotation-locked, it doesn't have any neighbors,
  /// all of its areas can rotate, and all of its rivers can rotate.
  bool can_rotate() const;

  /// Rotates the tile clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL if failed can_rotate check
  ///   - common::ERR_UNKNOWN otherwise
  common::Error rotate(int8_t rotations);

  /// Builds a road on the input border if possible.
  /// @param[in] border border to build a road on.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_INVALID if border is invalid value
  ///   - common::ERR_FAIL if unable to build road on this border
  common::Error build_road(const Border border);

  /// Builds a building on the input area if possible.
  /// @param[in] area Pointer to the area to construct a building on.
  /// @param[in] bldg Pointer to the building to construct.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - commmon::ERR_INVALID if area is invalid value, or bldg type is
  ///   invalid.
  ///   - common::ERR_FAIL otherwise
  common::Error build_building(const std::shared_ptr<Area> &area,
                               const std::shared_ptr<building::Building> &bldg);

  /// Builds a bridge at input river point if possible.
  /// @param[in] point Point to build a bridge.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL when unable to build bridge at input direction.
  ///   - common::ERR_INVALID on invalid direction.
  common::Error build_bridge(const Direction point);

  // Helpers
  Tile operator=(const Tile &other);
  bool operator==(Tile &other) const;
  bool operator!=(Tile &other) const;

  nlohmann::json to_json() const;

  friend std::ostream &operator<<(std::ostream &os, const tile::Tile &tile);

protected:
private:
  /// Divides areas based on where all river points are.
  void split_by_rivers();

  bool is_neighboring_sea() const;

  Hex m_p_hex;
  Terrain m_p_terrain;
  std::shared_ptr<Tile> m_p_neighbors[MAX_DIRECTIONS];
  std::vector<std::shared_ptr<River>> m_p_rivers;
  std::vector<std::shared_ptr<Area>> m_p_areas;
  std::pair<player::Color, uint8_t> m_p_walls[MAX_DIRECTIONS];
  // Flag to prevent tile from rotating after placed in a map.
  bool m_p_rot_locked;
  // Flag indicating whether the hex point has been set. If not, tile is not
  // fully initialized.
  bool m_p_hex_set;
};

/// Create a Tile object using the input json.
/// @param[in] j Input json
/// @param[out] t Pointer to created Tile object. Null on invalid json input.
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on invalid json object
static common::Error from_json(const nlohmann::json j,
                               std::shared_ptr<Tile> &t);
} // namespace tile

#endif // end Tile_H