#ifndef HELLOWORLD_APP_H
#define HELLOWORLD_APP_H

#include <escher.h>
#include "hello_controller.h"

namespace Helloworld {

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
  HelloController m_helloController;
};

}

#endif
