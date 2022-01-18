#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <portables/Resource.h>

using namespace portable;

static const std::string RESOURCE_NAMES[] = {
    "trunks", "boards",       "paper", "goose",  "clay",    "stone",   "fuel",
    "iron",   "gold",         "coins", "stock",  "bomb",    "manager", "pearl",
    "marble", "wood_carving", "ring",  "sketch", "pottery", "statue"};
static const size_t RESOURCE_NAMES_SIZE =
    sizeof RESOURCE_NAMES / sizeof RESOURCE_NAMES[0];

// Key used in JSON data objects
static const std::string KEY = "resource";

Resource::Resource(const Resource &other) : m_p_type(other.get_type()) {}
Resource::Resource(const Type &type) : m_p_type(type) {}

Resource::~Resource() {}

common::Error Resource::get_name(std::string &name) const
{
  common::Error err = common::ERR_FAIL;
  if (RESOURCE_NAMES_SIZE > m_p_type)
  {
    name = RESOURCE_NAMES[m_p_type];
    err = common::ERR_NONE;
  }
  return err;
}

common::Error Resource::to_json(nlohmann::json &j) const
{
  std::string name;
  common::Error err = get_name(name);
  if (common::ERR_NONE == err)
  {
    j[KEY] = name;
  }
  return err;
}

Resource Resource::operator=(const Resource &other) { return Resource(other); }
Resource Resource::operator=(const Type &other) { return Resource(other); }

bool Resource::operator==(Resource const &other) const
{
  return m_p_type == other.get_type();
}

bool Resource::operator==(Type const &t) const { return m_p_type == t; }