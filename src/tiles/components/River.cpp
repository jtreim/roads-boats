#include <memory>
#include <set>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/Transporter.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>
#include <tiles/components/River.h>
#include <utils/id_utils.h>

using namespace tile;

River::River(const std::set<Direction> river_points)
    : m_p_id(utils::gen_uuid()), m_p_points(river_points)
{
}

River::River(const River &other)
    : m_p_id(other.m_p_id), m_p_points(other.m_p_points),
      m_p_bridges(other.m_p_bridges)
{
  std::copy(other.m_p_transporters.begin(), other.m_p_transporters.end(),
            m_p_transporters.begin());
}

River::~River() {}

bool River::operator==(River const &other) const
{
  return m_p_id == other.m_p_id;
}

bool River::operator==(River &other) { return m_p_id == other.m_p_id; }

bool River::operator!=(River const &other) const { return !(*this == other); }

bool River::operator!=(River &other) { return !(*this == other); }

bool River::splits_borders(std::set<Border> borders) const
{
  bool does_split = false;
  for (auto point : m_p_points)
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
  if (m_p_points.size() > 1)
  {
    std::vector<Direction> p(m_p_points.begin(), m_p_points.end());

    // Pair up the right border of each point with the next point's left border.
    // Because it's circular, we know the last pairing will be whatever's
    // leftover.
    for (int i = 0; i < p.size() - 1; i++)
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
        for (int j = start; j < end; j++)
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

common::Error River::build_bridge(const Direction d)
{
  common::Error err = common::ERR_FAIL;
  if (!is_valid(d))
  {
    err = common::ERR_INVALID;
  }
  else if (can_build_bridge(d))
  {
    m_p_bridges.insert(d);
    err = common::ERR_NONE;
  }

  return err;
}

void River::rotate(int8_t rotations)
{
  // Making 0 rotations doesn't do anything...
  if (0 != rotations)
  {
    bool is_clockwise = (0 < rotations);
    rotations = abs(rotations) % MAX_DIRECTIONS;
    rotations = (is_clockwise ? rotations : (MAX_DIRECTIONS - rotations));

    std::vector<Direction> tmp_river_points;
    for (auto p : m_p_points)
    {
      Direction rotated =
          static_cast<Direction>((rotations + p) % MAX_DIRECTIONS);
      tmp_river_points.push_back(rotated);
    }
    m_p_points.clear();
    m_p_points.insert(tmp_river_points.begin(), tmp_river_points.end());

    std::vector<Direction> tmp_bridges;
    for (auto b : m_p_bridges)
    {
      Direction rotated =
          static_cast<Direction>((rotations + b) % MAX_DIRECTIONS);
      tmp_bridges.push_back(rotated);
    }
    m_p_bridges.clear();
    m_p_bridges.insert(tmp_bridges.begin(), tmp_bridges.end());
  }
}

nlohmann::json River::to_json() const
{
  nlohmann::json retval;
  retval["id"] = uuids::to_string(m_p_id);
  std::vector<nlohmann::json> points;
  for (Direction d : m_p_points)
  {
    points.push_back(json(d));
  }

  retval["points"] = points;

  std::vector<nlohmann::json> transporters;
  for (auto t : m_p_transporters)
  {
    if (t)
    {
      transporters.push_back(t->to_json());
    }
  }
  retval["transporters"] = transporters;

  std::vector<nlohmann::json> bridges;
  for (auto bridge : m_p_bridges)
  {
    nlohmann::json j;
    std::string name;
    std::vector<Border> borders = borders_from_direction(bridge);
    name = to_string(borders[0]) + "-" + to_string(borders[1]);
    j[name]["borders"] = borders;
    bridges.push_back(j);
  }
  retval["bridges"] = bridges;

  return retval;
}

// TODO: implement from_json