#ifndef ROCK_H
#define ROCK_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Rock : public Tile
{
public:
  Rock();

  Rock(std::set<Direction> p_river_points);

  ~Rock();

protected:
private:
};
} // namespace tile
#endif // end ROCK_H