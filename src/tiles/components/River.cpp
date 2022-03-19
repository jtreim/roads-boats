#include <memory>
#include <set>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/Transporter.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/River.h>
#include <utils/id_utils.h>

namespace tile
{

River::River() {}

River::River(const std::set<Direction> river_points) : m_points(river_points) {}

River::River(const River &other)
    : m_points(other.m_points), m_bridges(other.m_bridges)
{
}

River::~River()
{
  m_points.clear();
  reset();
}

void River::reset() { m_bridges.clear(); }

River River::operator=(River const &other)
{
  m_points = other.m_points;
  m_bridges = other.m_bridges;
  return (*this);
}

River River::operator=(River &other)
{
  m_points = other.m_points;
  m_bridges = other.m_bridges;
  return (*this);
}

bool River::operator==(River const &other) const
{
  return ((m_points == other.m_points) && (m_bridges == other.m_bridges));
}

bool River::operator==(River &other)
{
  return ((m_points == other.m_points) && (m_bridges == other.m_bridges));
}

bool River::operator!=(River const &other) const { return !(*this == other); }

bool River::operator!=(River &other) { return !(*this == other); }

bool River::splits_borders(std::set<Border> borders) const
{
  bool does_split = false;
  if (m_points.size() <= 1)
  {
    return false;
  }

  for (auto point : m_points)
  {
    std::vector<Border> b = borders_from_direction(point);
    if ((borders.contains(b[0])) && (borders.contains(b[1])))
    {
      does_split = true;
      break;
    }
  }
  return does_split;
}

std::vector<std::set<Border>>
River::get_area_borders(std::set<Border> borders) const
{
  std::vector<std::set<Border>> retval;
  if (m_points.size() > 1)
  {
    std::vector<Direction> p(m_points.begin(), m_points.end());

    // Pair up the right border of each point with the next point's left border.
    // Because it's circular, we know the last pairing will be whatever's
    // leftover.
    for (size_t i = 0; i < p.size() - 1; i++)
    {
      int next = i + 1;
      Border start = borders_from_direction(p[i])[1];
      Border end = borders_from_direction(p[next])[0];
      // Only add to potential areas if the river point splits available area
      if ((borders.contains(start)) && borders.contains(end))
      {
        std::set<Border> area_borders;
        area_borders.insert(start);
        borders.erase(start);
        area_borders.insert(end);
        borders.erase(end);
        // Fill in all borders between the two.
        // Only add the ones that we can from the borders remaining.
        for (size_t j = start; j < end; j++)
        {
          Border border = static_cast<Border>(j);
          if (borders.contains(border))
          {
            borders.erase(border);
            area_borders.insert(border);
          }
        }
        retval.push_back(area_borders);
      }
    }
  }
  retval.push_back(borders);

  return retval;
}

common::Error River::build(const Direction d)
{
  if (!is_valid(d))
  {
    return common::ERR_INVALID;
  }
  if (!can_build_bridge(d))
  {
    return common::ERR_FAIL;
  }

  m_bridges.insert(d);
  return common::ERR_NONE;
}

bool River::can_rotate() const
{
  // Don't allow rotating if the tile has any bridges
  return 0 == m_bridges.size();
}

common::Error River::rotate(int rotations)
{
  common::Error err = common::ERR_UNKNOWN;
  if (can_rotate())
  {
    // Making 0 rotations doesn't do anything...
    if (0 != rotations)
    {
      bool is_clockwise = (0 < rotations);
      rotations = abs(rotations) % MAX_DIRECTIONS;
      rotations = (is_clockwise ? rotations : (MAX_DIRECTIONS - rotations));

      std::vector<Direction> tmp_river_points;
      for (auto p : m_points)
      {
        Direction rotated =
            static_cast<Direction>((rotations + p) % MAX_DIRECTIONS);
        tmp_river_points.push_back(rotated);
      }
      m_points.clear();
      m_points.insert(tmp_river_points.begin(), tmp_river_points.end());
    }

    err = common::ERR_NONE;
  }
  else
  {
    err = common::ERR_FAIL;
  }
  return err;
}

std::ostream &operator<<(std::ostream &os, tile::River const &river)
{
  std::set<tile::Direction> pts = river.get_points();
  std::vector<tile::Direction> points(pts.begin(), pts.end());
  std::set<tile::Direction> brs = river.get_bridges();
  std::vector<tile::Direction> bridges(brs.begin(), brs.end());
  os << "<River::points=[" << points.at(0);
  for (size_t i = 1; i < points.size(); i++)
  {
    os << ", " << points.at(i);
  }
  os << "]";
  if (bridges.size() > 0)
  {
    os << ", bridges=[";
    os << bridges.at(0);
    for (size_t i = 1; i < bridges.size(); i++)
    {
      os << bridges.at(i);
    }
    os << "]";
  }
  os << ">";
  return os;
}

void to_json(nlohmann::json &j, const River &river)
{
  if (river.m_points.size() == 0)
  {
    j["points"] = nlohmann::json::array();
  }
  else
  {
    for (Direction d : river.m_points)
    {
      j["points"].push_back(to_string(d));
    }
  }

  if (river.m_bridges.size() == 0)
  {
    j["bridges"] = nlohmann::json::array();
  }
  else
  {
    for (auto bridge : river.m_bridges)
    {
      j["bridges"].push_back(to_string(bridge));
    }
  }
}

void from_json(const nlohmann::json &j, River &river)
{
  std::set<Direction> points;
  std::set<Direction> bridges;
  for (auto p : j.at("points").get<std::vector<Direction>>())
  {
    if (Direction::invalid_direction == p)
    {
      throw nlohmann::json::type_error::create(
          501, "Invalid direction value given as river point!", j);
    }
    points.insert(p);
  }

  river = River(points);

  for (auto b : j.at("bridges").get<std::vector<Direction>>())
  {
    if ((Direction::invalid_direction == b) || (!river.can_build_bridge(b)))
    {
      throw nlohmann::json::type_error::create(
          501, "Invalid direction value given as river bridge: " + to_string(b),
          j);
    }
    bridges.insert(b);
  }

  river.set_bridges(bridges);
}
} // namespace tile