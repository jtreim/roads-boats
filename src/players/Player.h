#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <memory>

#include <portables/transporters/Transporter.h>


namespace player
{
  class Transporter;
  class Land;

  /// Possible Player colors
  ///
  ///
  enum Color
  {
    BLACK,
    BLUE,
    GREEN,
    GREY,
    RED,
    YELLOW
  };

  /// Base Player class
  ///
  ///
  class Player
  {
    public:
      Color get_color() const;

      std::vector<std::unique_ptr<portable::Transporter>> get_transporters() const;

      bool create_transporter(portable::Transporter_type trans, Land location);

      bool remove_transporter(std::shared_ptr<portable::Transporter> transporter);

    protected:
    private:
      Color mp_color;
      std::vector<std::unique_ptr<portable::Transporter>> mp_transporters;
  };
}
#endif // end PLAYER_H