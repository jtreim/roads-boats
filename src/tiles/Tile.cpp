#include <map>
#include <memory>
#include <vector>

#include <buildings/Building.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

using namespace tile;

Tile::Tile() {}

Tile::Tile(std::set<Direction> p_river_points)
    : m_p_river_points(p_river_points)
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

std::shared_ptr<Tile> Tile::get_neighbor(Direction direction) const
{
  return m_p_neighbors[direction];
}

std::shared_ptr<Tile> *Tile::get_neighbors() { return m_p_neighbors; }

/// Checks whether tile has a river that flows through the direction
/// @param[in] direction
///
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
    if (neighbor->get_river_points().size() == m_max_directions)
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

std::shared_ptr<building::Building> Tile::get_building() const
{
  return m_p_building;
}

std::vector<std::shared_ptr<portable::Resource>> Tile::get_resources() const
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

/// Checks whether neighbor can be placed at the direction relative to the tile.
/// @param[in] neighbor  Tile to be placed
/// @param[in] direction  Direction neighbor would be to the tile
/// @return
///    - true if a neighbor can be added in the given direction
///    - false otherwise
bool Tile::can_add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction)
{
  bool is_allowed = true;
  if (m_p_neighbors[direction])
  {
    is_allowed = false;
  }

  Direction opposite = get_opposite_direction(direction);
  if (!neighbor->river_has_point(opposite))
  {
    is_allowed = false;
  }
  return is_allowed;
}