#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>

#include <tiles/components/Hex.h>

namespace utils
{
/// Creates the object based on the input JSON file
/// @param[in] p  JSON filepath to load
/// @param[out] result  Resulting object from file
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_NOT_FOUND if unable to locate file
///   - common::ERR_INVALID on invalid JSON
///   - common::ERR_FAIL on any other errors
template <class T>
common::Error load_json(const std::filesystem::path p, T &result)
{
  common::Error err = common::ERR_FAIL;
  if (!std::filesystem::exists(p))
  {
    err = common::ERR_NOT_FOUND;
  }
  else
  {
    std::ifstream in(p.string().c_str());
    if (in.is_open())
    {
      try
      {
        nlohmann::json j;
        in >> j;
        result = j.get<T>();
        err = common::ERR_NONE;
      }
      catch (nlohmann::json::exception e)
      {
        // Invalid json format
        // TODO: Log error
        // std::cout << e.what() << std::endl;
        err = common::ERR_INVALID;
      }
      catch (...)
      {
        // TODO: Log error
        err = common::ERR_FAIL;
      }
    }
  }

  return err;
};

/// Dumps the object to the input JSON file
/// @param[in] p  Filepath to save object to
/// @param[in] object  Object to save to JSON file
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on failure
template <class T>
common::Error dump_json(const std::filesystem::path p, const T object)
{
  common::Error err = common::ERR_UNKNOWN;
  nlohmann::json j = object;
  try
  {
    std::ofstream out(p.string().c_str());
    out << j;
    err = common::ERR_NONE;
  }
  catch (...)
  {
    err = common::ERR_FAIL;
  }
  return err;
};

} // namespace utils
#endif