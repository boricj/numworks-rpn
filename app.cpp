#include "app.h"
#include "apps/apps_container.h"
#include "rpn_icon.h"
#include "apps/i18n.h"
#include <assert.h>

extern const ToolboxMessageTree *toolboxModel;

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
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

Stack * App::Snapshot::stack() {
  return &m_stack;
}

void App::Snapshot::tidy() {
}

void App::Snapshot::reset() {
  m_stack(Stack::CLEAR);
}

App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_inputController),
  m_stackController(this, snapshot->stack(), &m_inputController, &m_view, localContext()),
  m_inputController(this, snapshot->stack(), &m_stackController, &m_view),
  m_view(this, &m_inputController, &m_stackController),
  m_toolbox(AppsContainer::sharedAppsContainer()->mathToolbox()->rootModel(), &m_inputController, &m_stackController)
{
}

::Toolbox * App::toolboxForInputEventHandler(InputEventHandler * textInput) {
  return &m_toolbox;
}

}
