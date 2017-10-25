#include "app.h"
#include "apps/apps_container.h"
#include "helloworld_icon.h"
#include "apps/i18n.h"
#include <assert.h>

namespace Helloworld {

I18n::Message App::Descriptor::name() {
  return I18n::Message::HelloApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::HelloAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::HelloworldIcon;
}

App::Snapshot::Snapshot()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_helloController),
  m_helloController(this)
{
}

}
