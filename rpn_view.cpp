#include "rpn_view.h"

#include "apps/i18n.h"

namespace Rpn {

RpnView::RpnView() :
  View(),
  m_bufferTextView(KDText::FontSize::Large, 0.5, 0.5, KDColorBlack)
{
  m_bufferTextView.setText(I18n::translate(I18n::Message::RpnApp));
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
}

void RpnView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), Palette::GreyWhite);
}

void RpnView::reload() {
  m_bufferTextView.setBackgroundColor(Palette::GreyWhite);
  markRectAsDirty(bounds());
}

int RpnView::numberOfSubviews() const {
  return 1;
}

View * RpnView::subviewAtIndex(int index) {
  return &m_bufferTextView;
}

void RpnView::layoutSubviews() {
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
}

}
