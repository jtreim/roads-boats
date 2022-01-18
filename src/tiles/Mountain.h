#ifndef MOUNTAIN_H
#define MOUNTAIN_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Mountain : public Tile
{
public:
  Mountain();

  Mountain(std::set<Direction> p_river_points);

  ~Mountain();

protected:
private:
};
} // namespace tile
#endif // end MOUNTAIN_H