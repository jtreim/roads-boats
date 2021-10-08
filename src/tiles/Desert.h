#ifndef DESERT_H
#define DESERT_H

#include <memory>
#include <set>

#include "tiles/Tile.h"

namespace tile
{
class Desert : public Tile
{
public:
  Desert();

  Desert(std::set<Direction> p_river_points);

  ~Desert();

  bool add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction);

  bool add_neighbor(std::shared_ptr<Tile> neighbor);

protected:
private:
};
} // namespace tile
#endif // end DESERT_H