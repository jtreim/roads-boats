#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

namespace portable
{

class Transporter
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

  Transporter();
  Transporter(Type m_type);
  Transporter(const Transporter &other);

  virtual ~Transporter();

  Transporter operator=(const Transporter &other);

  bool operator==(const Transporter &other) { return m_p_id == other.m_p_id; }

  inline Type get_type() const { return m_type; }

  nlohmann::json to_json() const;

protected:
private:
  uuids::uuid m_p_id;
  Type m_type;
};
} // namespace portable
#endif // end TRANSPORTER_H