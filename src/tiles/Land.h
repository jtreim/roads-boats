#ifndef LAND_H
#define LAND_H

#include <buildings/Building.h>
#include <players/Player.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace tile
{
  enum Direction
  {
    north,
    north_east,
    south_east,
    south,
    south_west,
    north_west
  };

  class Land
  {
    public:
      Land();
      Land(std::set<Direction> p_river_points);
      ~Land();

      virtual bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction) = 0;

      virtual bool add_neighbor(std::shared_ptr<Land> neighbor) = 0;

      std::shared_ptr<Land> get_neighbor(Direction direction) const;

      std::shared_ptr<Land>* get_neighbors();

      bool river_has_point(Direction direction) const;

      std::set<Direction> get_river_points() const;
      
      bool is_neighboring_sea() const;

      bool is_shore() const;

      std::shared_ptr<building::Building> get_building() const;

      std::vector<std::shared_ptr<portable::Resource>> get_resources() const;

      std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>> get_transporters() const;

      std::vector<std::shared_ptr<portable::Transporter>> get_player_transporters(player::Color color) const;

      static Direction get_opposite_direction(Direction direction);

      static const uint8_t m_max_directions = 6;

    protected:
      std::set<Direction> m_p_river_points;
      std::shared_ptr<Land> m_p_neighbors[6];
      std::shared_ptr<building::Building> m_p_building;
      std::vector<std::shared_ptr<portable::Resource>> m_p_resources;
      std::map<player::Color, std::vector<std::shared_ptr<portable::Transporter>>> m_p_transporters;
      bool can_add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

    private:
  };
}
#endif // end LAND_H