#ifndef RPN_APP_H
#define RPN_APP_H

#include <escher.h>
#include "rpn_controller.h"

namespace Rpn {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
  };
private:
  App(Container * container, Snapshot * snapshot);
  RpnController m_rpnController;
};

}

#endif
