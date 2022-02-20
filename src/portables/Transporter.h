#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

namespace portable
{

class Transporter
{
public:
  Transporter();
  Transporter(const Transporter &other);

  virtual ~Transporter();

  Transporter operator=(const Transporter &other);

  bool operator==(const Transporter &other) { return m_p_id == other.m_p_id; }

  nlohmann::json to_json() const;

protected:
private:
  uuids::uuid m_p_id;
};
} // namespace portable
#endif // end TRANSPORTER_H