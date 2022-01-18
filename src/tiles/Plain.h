#ifndef PLAIN_H
#define PLAIN_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Plain : public Tile
{
public:
  Plain();

  Plain(std::set<Direction> p_river_points);

  ~Plain();

protected:
private:
};
} // namespace tile
#endif // end PLAIN_H