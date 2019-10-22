#include "rpn_content_view.h"
#include "rpn_input_controller.h"
#include "rpn_stack_controller.h"

namespace Rpn {

ContentView::ContentView(Responder * parentResponder, InputController * inputController, StackController * stackController) :
  View(),
  m_stackView(stackController, stackController, stackController),
  m_inputView(parentResponder, m_textBuffer, sizeof(m_textBuffer), sizeof(m_textBuffer), inputController, inputController, KDFont::LargeFont),
  m_textBuffer("")
{
}

ContentView::~ContentView() {
}

View * ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  switch (index) {
    case 0:  return &m_stackView;
    default: return &m_inputView;
  }
}

void ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = 32;
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_stackView.setFrame(mainViewFrame);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
  m_inputView.setLeftMargin(4);
  m_inputView.setFrame(inputViewFrame);
}

void ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

}
