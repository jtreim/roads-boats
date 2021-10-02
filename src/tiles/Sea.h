#ifndef SEA_H
#define SEA_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Sea : public Tile
{
public:
  Sea();

  Sea(std::set<Direction> p_river_points);

  ~Sea();

  bool add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction);

  bool add_neighbor(std::shared_ptr<Tile> neighbor);

  bool is_shore() const;

protected:
private:
};
} // namespace tile
#endif // end SEA_H