#ifndef TILE_H
#define TILE_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/transporters/Transporter.h>

namespace tile
{
enum Direction
{
  north_east = 0,
  east,
  south_east,
  south_west,
  west,
  north_west
};

class Tile : public std::enable_shared_from_this<Tile>
{
public:
  Tile();
  Tile(std::set<Direction> p_river_points);
  ~Tile();

  bool operator==(Tile const &other) const;

  // Abstract calls
  virtual nlohmann::json to_json() const;

  // getters
  uint8_t get_id() const { return id; };

  /// Gets the first adjacent tile in the input direction
  /// @return
  ///    - pointer to the first adjacent tile
  ///    - nullptr if no tile is in the given direction
  std::shared_ptr<Tile> get_neighbor(Direction direction) const;
  std::shared_ptr<Tile> *get_neighbors();
  std::set<Direction> get_river_points() const;

  /// Gets the building (if any) currently on this tile
  /// @return
  ///    - pointer to the structure built on this tile
  ///    - nullptr if no structure has been built here
  std::shared_ptr<building::Building> get_building() const;
  std::map<std::string, std::pair<portable::Resource, uint8_t>>
  get_resources() const;
  std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
  get_transporters() const;
  std::vector<std::shared_ptr<portable::Transporter>>
  get_player_transporters(player::Color color) const;

  /// Checks whether tile has a river that flows through the input direction
  /// @param[in] direction
  /// @return
  ///    - true if the tile has a river flowing through the input direction
  ///    - false otherwise
  bool river_has_point(Direction direction) const;
  bool is_neighboring_sea() const;
  virtual bool is_shore() const;
  virtual inline bool is_sea() const { return false; }

  /// Add a tile as a neighbor in the input direction.
  /// @param[in] neighbor The tile to be placed next to this one.
  /// @param[in] direction The direction the tile should be placed from first
  /// tile's perspective.
  /// @return
  ///    - common::Error::ERR_INVALID if either param is an invalid format
  ///    (null, nonexistant direction).
  ///    - common::Error::ERR_FAIL if there's already a neighbor in the given
  ///    direction, or the neighbor's river points doesn't allow being added
  ///    there.
  ///    - common::Error::ERR_NONE on success.
  virtual common::Error add_neighbor(std::shared_ptr<Tile> neighbor,
                                     Direction direction);

  /// Remove the neighbor from the input direction
  /// @param[in] neighbor The direction of the neighbor to be removed.
  /// @return
  ///    - common::Error::ERR_INVALID if the direction is invalid.
  ///    - common::Error::ERR_MISSING if there is no neighbor in the input
  ///    direction.
  ///    - common::Error::ERR_NONE on success.
  virtual common::Error remove_neighbor(Direction direction);

  /// Removes all neighbors from the tile
  /// @return
  ///    - common::Error::ERR_NONE on success
  ///    - common::Error::ERR_FAIL on fail
  common::Error clear_neighbors();

  // Helpers
  // friend std::ostream &operator<<(std::ostream &os, const Tile &tile);
  inline static bool is_valid_direction(Direction direction)
  {
    return ((0 <= direction) && (m_max_directions > direction));
  };

  static Direction get_opposite_direction(Direction direction);
  static const uint8_t m_max_directions = 6;

  static uint8_t unique_tile_id;

protected:
  /// Checks whether neighbor can be placed at the direction relative to the
  /// tile.
  /// @param[in] neighbor  Tile to be placed
  /// @param[in] direction  Direction neighbor would be to the tile
  /// @return
  ///    - common::Error::ERR_INVALID if either param is an invalid format
  ///    (null, nonexistant direction).
  ///    - common::Error::ERR_FAIL if there's already a neighbor in the given
  ///    direction, the neighbor's river points doesn't allow being added
  ///    there, or if the new neighbor matches ourselves or a neighbor we
  ///    already have.
  ///    - common::Error::ERR_NONE on valid placement.
  virtual common::Error can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                         Direction direction);

  uint8_t id;
  std::shared_ptr<Tile> m_p_neighbors[6];
  std::set<Direction> m_p_river_points;
  std::shared_ptr<building::Building> m_p_building;
  std::map<std::string, std::pair<portable::Resource, uint8_t>> m_p_resources;
  std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
      m_p_transporters;

private:
};

} // namespace tile
#endif // end Tile_H