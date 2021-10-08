#ifndef MOUNTAIN_H
#define MOUNTAIN_H

#include <memory>
#include <set>

#include "tiles/Tile.h"

namespace tile
{
class Mountain : public Tile
{
public:
  Mountain();

  Mountain(std::set<Direction> p_river_points);

  ~Mountain();

  bool add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction);

  bool add_neighbor(std::shared_ptr<Tile> neighbor);

protected:
private:
};
} // namespace tile
#endif // end MOUNTAIN_H