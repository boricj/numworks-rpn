#include "hello_view.h"

namespace boricj {
namespace helloworld {

HelloView::HelloView() :
  View(),
  m_color(0)
{
}

void HelloView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor color;
  switch (m_color) {
    case 0:
      color = Palette::GreyDark;
      break;
    case 1:
      color = Palette::GreyMiddle;
      break;
    case 2:
      color = Palette::GreyBright;
      break;
    default:
      color = Palette::GreyWhite;
  }

  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), color);
}

void HelloView::reload() {
  markRectAsDirty(bounds());
}

void HelloView::changeColor() {
  m_color++;
  if (m_color > 3)
    m_color = 0;

  reload();
}

}
}
