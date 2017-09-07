#ifndef BORICJ_HELLOWORLD_APP_H
#define BORICJ_HELLOWORLD_APP_H

#include <escher.h>
#include "hello_controller.h"

namespace boricj {
namespace helloworld {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    const char * uriName() override;
    const I18n::Message *name() override;
    const I18n::Message *upperName() override;
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
  HelloController m_helloController;
};

}
}

#endif
