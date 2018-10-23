#ifndef RPN_APP_H
#define RPN_APP_H

#include <escher.h>
#include "../shared/text_field_delegate_app.h"
#include "rpn_prompt_controller.h"
#include "rpn_stack.h"

namespace Rpn {

class App : public Shared::TextFieldDelegateApp {
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
    RpnStack * rpnStack();
  private:
    void tidy() override;
    RpnStack m_rpnStack;
  };
private:
  App(Container * container, Snapshot * snapshot);
  RpnPromptController m_rpnPromptController;
};

}

#endif
