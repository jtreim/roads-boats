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

River::River(const std::set<Direction> river_points,
             const std::set<std::shared_ptr<Area>> areas)
    : m_p_id(utils::gen_uuid()), m_p_points(river_points), m_p_areas(areas)
{
}

River::River(const River &other)
    : m_p_id(other.m_p_id), m_p_points(other.m_p_points),
      m_p_areas(other.m_p_areas), m_p_bridges(other.m_p_bridges)
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

std::vector<std::shared_ptr<Area>> River::get_accessible_areas() {}

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
    // j[name]["areas"].push_back(
    //     uuids::to_string(m_p_parent->get_area(borders[0])->get_id()));
    // j[name]["areas"].push_back(
    //     uuids::to_string(m_p_parent->get_area(borders[1])->get_id()));
    bridges.push_back(j);
  }
  retval["bridges"] = bridges;

  return retval;
}

// TODO: implement from_json