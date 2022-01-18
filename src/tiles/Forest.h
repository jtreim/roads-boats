#ifndef FOREST_H
#define FOREST_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Forest : public Tile
{
public:
  Forest();

  Forest(std::set<Direction> p_river_points);

  ~Forest();

protected:
private:
};
} // namespace tile
#endif // end FOREST_H