#ifndef RPN_RPN_PROMPT_CONTROLLER_H
#define RPN_RPN_PROMPT_CONTROLLER_H

#include <escher.h>

#include "../shared/text_field_delegate.h"
#include "rpn_stack_controller.h"
#include "rpn_stack.h"

namespace Rpn {

class RpnPromptController : public ViewController, public Shared::TextFieldDelegate {
public:
  RpnPromptController(Responder * parentResponder, RpnStack * rpnStack);
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
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, RpnStackController * stackController, TextFieldDelegate * delegate);
    ~ContentView();
    ContentView(const ContentView& other) = delete;
    ContentView(ContentView&& other) = delete;
    ContentView& operator=(const ContentView& other) = delete;
    ContentView& operator=(ContentView&& other) = delete;
    void reload();
    SelectableTableView * mainView() { return &m_mainView; }
    TextField * promptView() { return &m_promptView; }
    /* View */
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    
  private:
    SelectableTableView m_mainView;
    TextField m_promptView;
    char m_textBuffer[TextField::maxBufferSize()];
  };

  bool handleEventOperation(Ion::Events::Event event);
  bool handleEventSpecial(Ion::Events::Event event);

private:
  RpnStack *m_rpnStack;
  ContentView m_view;
  RpnStackController m_stackController;
};

}

#endif
