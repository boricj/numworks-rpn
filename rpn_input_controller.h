#ifndef RPN_INPUT_CONTROLLER_H
#define RPN_INPUT_CONTROLLER_H

#include <escher.h>

#include "../shared/text_field_delegate.h"
#include "../shared/input_event_handler_delegate.h"
//#include "rpn_input_field.h"
#include "rpn_stack_controller.h"
#include "rpn_stack.h"

namespace Rpn {
class ContentView;
class StackController;

class InputController : public ViewController, public Shared::TextFieldDelegate, public Shared::InputEventHandlerDelegate {
public:
  InputController(Responder * parentResponder, Stack * stack, StackController * stackController, ContentView * view);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) override;

  bool pushInput();
  void setText(const char *text);

public:
  bool handleEventOperation(Ion::Events::Event event, TextField * view);
  bool handleEventSpecial(Ion::Events::Event event, TextField * view);
  TextField* inputView();

private:
  Stack *m_stack;
  StackController *m_stackController;
  ContentView *m_view;
};

}

#endif
