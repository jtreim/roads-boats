#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <nlohmann/json.hpp>

#include <players/Color.h>
#include <portables/Portable.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>

namespace portable
{
class Transporter : public Portable
{
public:
  enum Type
  {
    invalid = -1,
    donkey = 0,
    wagon,
    truck,
    raft,
    rowboat,
    steamer
  };

  static bool can_travel_on_sea(const Type t)
  {
    return ((Type::raft == t) || (Type::rowboat == t) || (Type::steamer == t));
  }

  Transporter();
  Transporter(const Transporter::Type type, const player::Color color,
              tile::Tile *tile, const tile::Border entered_border,
              const uint8_t max_capacity, const uint8_t remaining_movement);
  Transporter(const Transporter &other);

  virtual ~Transporter();

  virtual void reset();

  void clean();

  bool can_pickup_resource(portable::Resource *res);
  common::Error pickup_resource(portable::Resource *&res);
  bool can_drop_resource(const size_t idx,
                         tile::Border border = tile::Border::invalid_border);
  virtual common::Error
  drop_resource(const size_t idx,
                tile::Border border = tile::Border::invalid_border);
  inline const std::vector<portable::Resource *> get_cargo() { return m_cargo; }

  virtual bool can_pickup_transporter(const Transporter *other);
  virtual common::Error pickup_transporter(Transporter *other);
  virtual bool can_drop_transporter(const tile::Border border);
  virtual common::Error drop_transporter(const tile::Border border);

  inline player::Color get_color() const { return m_color; }
  inline Type get_type() const { return m_type; }

  virtual void end_movement() { m_remaining_movement = 0; }
  virtual bool can_move_to(const tile::Tile *next_tile,
                           const tile::Border border) = 0;
  virtual common::Error move_to(const tile::Tile *next_tile,
                                const tile::Border border) = 0;

  nlohmann::json to_json() const;

protected:
  Type m_type;
  player::Color m_color;

  tile::Tile *m_tile;
  tile::Border m_entered_border;

  std::vector<portable::Resource *> m_cargo;
  portable::Transporter *m_carried_transporter;
  uint8_t m_max_capacity;

  uint8_t m_remaining_movement;
  uint8_t m_max_movement;

private:
};
} // namespace portable
#endif // end TRANSPORTER_H