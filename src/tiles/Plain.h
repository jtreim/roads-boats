#ifndef PLAIN_H
#define PLAIN_H

#include <memory>
#include <set>

#include <tiles/Land.h>


namespace tile
{
  class Plain : public Land
  {
    public:
      Plain();

      Plain(std::set<Direction> p_river_points);

      ~Plain();

      bool add_neighbor(std::shared_ptr<Land> neighbor, Direction direction);

      bool add_neighbor(std::shared_ptr<Land> neighbor);

    protected:
    private:
  };
}
#endif // end PLAIN_H