#ifndef FOREST_H
#define FOREST_H

#include <memory>
#include <set>

#include "tiles/Tile.h"

namespace tile
{
class Forest : public Tile
{
public:
  Forest();

  Forest(std::set<Direction> p_river_points);

  ~Forest();

  bool add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction);

  bool add_neighbor(std::shared_ptr<Tile> neighbor);

protected:
private:
};
} // namespace tile
#endif // end FOREST_H