#include "hello_view.h"

namespace boricj {
namespace helloworld {

HelloView::HelloView() :
  View(),
  m_bufferTextView(KDText::FontSize::Large, 0.5, 0.5, KDColorBlack),
  m_color(3),
  m_kdcolor(Palette::GreyWhite)
{
  m_bufferTextView.setText("Hello, World!");
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
}

void HelloView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_kdcolor);
}

void HelloView::reload() {
  switch (m_color) {
    case 0:
      m_kdcolor = Palette::GreyDark;
      break;
    case 1:
      m_kdcolor = Palette::GreyMiddle;
      break;
    case 2:
      m_kdcolor = Palette::GreyBright;
      break;
    default:
      m_kdcolor = Palette::GreyWhite;
  }

  m_bufferTextView.setBackgroundColor(m_kdcolor);

  markRectAsDirty(bounds());
}

void HelloView::changeColor() {
  m_color++;
  if (m_color > 3)
    m_color = 0;

  reload();
}

int HelloView::numberOfSubviews() const {
  return 1;
}

View * HelloView::subviewAtIndex(int index) {
  return &m_bufferTextView;
}

void HelloView::layoutSubviews() {
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
}

}
}
