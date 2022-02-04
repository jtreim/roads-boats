#ifndef PLAIN_H
#define PLAIN_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Plains : public Tile
{
public:
  Plains();

  Plains(std::set<Direction> p_river_points);

  ~Plains();

protected:
private:
};
} // namespace tile
#endif // end PLAIN_H