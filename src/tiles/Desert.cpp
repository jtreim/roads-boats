#include <memory>
#include <set>
#include <vector>

#include <tiles/Desert.h>

using namespace tile;

Desert::Desert() {}

Desert::Desert(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Desert::~Desert() {}

// nlohmann::json Desert::to_json()
// {

// }