#include "app.h"
#include "apps/apps_container.h"
#include "rpn_icon.h"
#include "apps/i18n.h"
#include <assert.h>

namespace Rpn {

I18n::Message App::Descriptor::name() {
  return I18n::Message::RpnApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::RpnAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::RpnIcon;
}

App::Snapshot::Snapshot()
{
}

App * App::Snapshot::unpack(Container * container) {
  App * app = new App(container, this);
  return app;
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

RpnStack * App::Snapshot::rpnStack() {
  return &m_rpnStack;
}

void App::Snapshot::tidy() {
}

void App::Snapshot::reset() {
  m_rpnStack.clear();
}

App::App(Container * container, Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_rpnPromptController),
  m_rpnPromptController(this, snapshot->rpnStack())
{
}

}
