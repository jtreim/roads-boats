#ifndef TILE_H
#define TILE_H

#include <buildings/Building.h>
#include <players/Player.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace base
{
namespace tile
{
enum Direction
{
  north_east,
  east,
  south_east,
  south_west,
  west,
  north_west
};

class Tile
{
public:
  Tile();
  Tile(std::set<Direction> p_river_points);
  ~Tile();

  // Abstract calls
  virtual bool add_neighbor(std::shared_ptr<Tile> neighbor,
                            Direction direction) = 0;
  virtual bool add_neighbor(std::shared_ptr<Tile> neighbor) = 0;

  // getters
  std::shared_ptr<Tile> get_neighbor(Direction direction) const;
  std::shared_ptr<Tile> *get_neighbors();
  std::set<Direction> get_river_points() const;
  std::shared_ptr<building::Building> get_building() const;
  std::vector<std::shared_ptr<portable::Resource>> get_resources() const;
  std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
  get_transporters() const;
  std::vector<std::shared_ptr<portable::Transporter>>
  get_player_transporters(player::Color color) const;
  // bool tests
  bool river_has_point(Direction direction) const;
  bool is_neighboring_sea() const;
  bool is_shore() const;

  // Helpers
  friend std::ostream &operator<<(std::ostream &os, const Tile &tile);
  std::string json();

  // static
  static Direction get_opposite_direction(Direction direction);
  static const uint8_t m_max_directions = 6;

protected:
  // Function
  virtual bool can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                Direction direction);

  std::shared_ptr<Tile> m_p_neighbors[6];
  std::set<Direction> m_p_river_points;
  std::shared_ptr<building::Building> m_p_building;
  std::vector<std::shared_ptr<portable::Resource>> m_p_resources;
  std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>>
      m_p_transporters;

private:
};

} // namespace tile
} // namespace base
#endif // end Tile_H