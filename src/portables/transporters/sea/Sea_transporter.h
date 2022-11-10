#ifndef SEA_TRANSPORTER_H
#define SEA_TRANSPORTER_H

#include <nlohmann/json.hpp>

#include <players/Color.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>
#include <tiles/components/Border.h>

namespace portable
{

class Sea_transporter : public Transporter
{
public:
  Sea_transporter();
  Sea_transporter(const Type type, const player::Color color, tile::Tile *tile,
                  const tile::Border entered_border, const uint8_t max_capacity,
                  const uint8_t remaining_movement, const bool just_docked);
  Sea_transporter(const Sea_transporter &other);

  virtual ~Sea_transporter();

  virtual void reset();

  virtual bool
  can_drop_resource(const size_t idx,
                    tile::Border border = tile::Border::invalid_border);
  virtual common::Error
  drop_resource(const size_t idx,
                tile::Border border = tile::Border::invalid_border);

  virtual bool can_pickup_transporter(const Transporter *other);
  virtual common::Error pickup_transporter(Transporter *other);
  virtual bool can_drop_transporter(const tile::Border border);
  virtual common::Error drop_transporter(const tile::Border border);
  virtual void end_movement()
  {
    m_remaining_movement = 0;
    m_just_docked = true;
  }
  bool can_move_to(const tile::Tile *next_tile, const tile::Border border);
  common::Error move_to(const tile::Tile *next_tile, const tile::Border border);

protected:
private:
  bool m_just_docked;
};
} // namespace portable
#endif // end SEA_TRANSPORTER_H