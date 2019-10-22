#ifndef RPN_CONTENT_VIEW_H
#define RPN_CONTENT_VIEW_H

#include <escher.h>
#include "../shared/text_field_delegate_app.h"

namespace Rpn {
  class StackController;
  class InputController;

class ContentView : public View {
public:
  ContentView(Responder * parentResponder, InputController * inputController, StackController * stackController);
  ~ContentView();
  ContentView(const ContentView& other) = delete;
  ContentView(ContentView&& other) = delete;
  ContentView& operator=(const ContentView& other) = delete;
  ContentView& operator=(ContentView&& other) = delete;
  void reload();
  SelectableTableView * stackView() { return &m_stackView; }
  TextField * inputView() { return &m_inputView; }
  /* View */
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

private:
  SelectableTableView m_stackView;
  TextField m_inputView;
  char m_textBuffer[TextField::maxBufferSize()];
};

}

#endif
