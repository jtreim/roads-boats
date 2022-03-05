#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/Transporter.h>
#include <utils/id_utils.h>

using namespace portable;

Transporter::Transporter() : m_p_id(utils::gen_uuid()) {}

Transporter::Transporter(const Transporter &other) : m_p_id(other.m_p_id) {}

Transporter::~Transporter() {}

Transporter Transporter::operator=(const Transporter &other)
{
  return Transporter(other);
}

nlohmann::json Transporter::to_json() const
{
  // TODO: Dump transporter data to json
  nlohmann::json retval;
  return retval;
}