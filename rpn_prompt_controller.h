#ifndef RPN_RPN_PROMPT_CONTROLLER_H
#define RPN_RPN_PROMPT_CONTROLLER_H

#include <escher.h>

#include "rpn_stack_controller.h"
#include "rpn_stack.h"

namespace Rpn {

class RpnPromptController : public ViewController {
public:
  RpnPromptController(Responder * parentResponder, RpnStack * rpnStack);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

public:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, RpnStackController * stackController);
    ~ContentView();
    ContentView(const ContentView& other) = delete;
    ContentView(ContentView&& other) = delete;
    ContentView& operator=(const ContentView& other) = delete;
    ContentView& operator=(ContentView&& other) = delete;
    void reload();
    SelectableTableView * mainView() { return &m_mainView; }
    BufferTextView * promptView() { return &m_promptView; }
    /* View */
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
  private:
    SelectableTableView m_mainView;
    BufferTextView m_promptView;
  };

  void handleEventEXE();

  static constexpr int k_bufferLength = TextField::maxBufferSize();
  char m_textBody[k_bufferLength];
  char *m_curTextPtr;
  RpnStack *m_rpnStack;
  ContentView m_view;
  RpnStackController m_stackController;
};

}

#endif
