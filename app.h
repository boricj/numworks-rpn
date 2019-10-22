#ifndef RPN_APP_H
#define RPN_APP_H

#include <escher.h>
#include "../shared/text_field_delegate_app.h"
#include "rpn_content_view.h"
#include "rpn_stack.h"
#include "rpn_stack_controller.h"
#include "rpn_input_controller.h"
#include "rpn_toolbox.h"

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
    Stack * stack();
  private:
    void tidy() override;
    Stack m_stack;
  };
private:
  App(Snapshot * snapshot);
  ::Toolbox * toolboxForInputEventHandler(InputEventHandler * textInput) override;

  StackController m_stackController;
  InputController m_inputController;
  ContentView m_view;
  Toolbox m_toolbox;
};

}

#endif
