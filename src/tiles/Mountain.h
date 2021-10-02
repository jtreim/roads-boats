#ifndef MOUNTAIN_H
#define MOUNTAIN_H

#include <memory>
#include <set>

#include <tiles/Land.h>


namespace tile
{
  class Mountain : public Land
  {
    public:
      Mountain();

      Mountain(std::set<Direction> p_river_points);

      ~Mountain();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

    protected:
    private:
  };
}
#endif // end MOUNTAIN_H