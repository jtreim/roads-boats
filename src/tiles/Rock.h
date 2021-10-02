#ifndef ROCK_H
#define ROCK_H

#include <memory>
#include <set>

#include <tiles/Land.h>


namespace tile
{
  class Rock : public Land
  {
    public:
      Rock();

      Rock(std::set<Direction> p_river_points);

      ~Rock();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

    protected:
    private:
  };
}
#endif // end ROCK_H