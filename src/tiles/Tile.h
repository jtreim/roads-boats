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
#include <tiles/components/Hex_point.h>
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

class Tile : public std::enable_shared_from_this<Tile>
{
public:
  Tile(const Terrain t = Terrain::desert);
  Tile(const hex_point hp, const Terrain t = Terrain::desert);
  Tile(const hex_point hp, const std::vector<River> rivers,
       const Terrain t = Terrain::desert);
  Tile(const Tile &other);
  Tile();
  ~Tile();

  inline uuids::uuid get_id() const { return m_p_id; };
  inline Terrain get_terrain() const { return m_p_terrain; }
  inline hex_point get_hex_point() { return m_p_hex_point; };
  inline std::vector<River> get_rivers() const { return m_p_rivers; }
  inline std::vector<Area> const get_all_areas() const { return m_p_areas; }
  std::shared_ptr<Area> get_area(const Border b) const;
  std::vector<std::shared_ptr<Area>> get_areas(const Direction d) const;

  /// Gets the first adjacent tile in the input direction
  /// @param direction Side of the tile to check for a neighbor
  /// @return
  ///   - pointer to the first adjacent tile
  ///   - nullptr if no tile is in the given direction
  std::shared_ptr<Tile> get_neighbor(const Direction direction) const;
  std::shared_ptr<Tile> *get_neighbors();
  inline std::pair<player::Color, uint8_t> get_wall(const Direction d) const
  {
    if (is_valid(d))
    {
      return m_p_walls[d];
    }
    return std::make_pair<player::Color, uint8_t>(player::Color::invalid, 0);
  }
  std::set<Direction> get_all_river_points() const;

  /// Gets the building (if any) currently on this tile
  /// @return
  ///   - pointer to the structure built on this tile
  ///   - nullptr if no structure has been built here
  std::shared_ptr<building::Building> get_building() const;
  std::map<std::string, std::pair<portable::Resource, uint8_t>>
  get_all_resources() const;
  std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
  get_all_transporters() const;
  std::vector<std::shared_ptr<portable::Transporter>>
  get_all_player_transporters(player::Color color) const;

  /// Checks whether tile has a river that flows through the input direction
  /// @param[in] direction
  /// @return
  ///   - true if the tile has a river flowing through the input direction
  ///   - false otherwise
  bool has_river_point(const Direction direction) const;
  bool is_shore() const;

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
  virtual common::Error add_neighbor(std::shared_ptr<Tile> neighbor,
                                     Direction direction);

  /// Remove the neighbor from the input direction
  /// @param[in] neighbor The direction of the neighbor to be removed.
  /// @return
  ///   - common::Error::ERR_INVALID if the direction is invalid.
  ///   - common::Error::ERR_MISSING if there is no neighbor in the input
  ///   direction.
  ///   - common::Error::ERR_NONE on success.
  virtual common::Error remove_neighbor(Direction direction);

  /// Removes all neighbors from the tile
  /// @return
  ///   - common::Error::ERR_NONE on success
  ///   - common::Error::ERR_FAIL on fail
  common::Error clear_neighbors();

  /// Rotates the tile clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise.
  void rotate(int8_t rotations);

  // Helpers
  // friend std::ostream &operator<<(std::ostream &os, const Tile &tile);
  bool operator==(Tile &other) const;
  bool operator!=(Tile &other) const;

  nlohmann::json to_json() const;

protected:
private:
  /// Divides areas based on where all river points are.
  void split_by_rivers();

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

  bool is_neighboring_sea() const;

  uuids::uuid m_p_id;
  hex_point m_p_hex_point;
  Terrain m_p_terrain;
  std::shared_ptr<Tile> m_p_neighbors[MAX_DIRECTIONS];
  std::vector<River> m_p_rivers;
  std::vector<Area> m_p_areas;
  std::pair<player::Color, uint8_t> m_p_walls[MAX_DIRECTIONS];
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