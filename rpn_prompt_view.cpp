#include "rpn_prompt_view.h"

#include "apps/i18n.h"

namespace Rpn {

RpnPromptView::RpnPromptView() :
  View(),
  m_bufferTextView(KDText::FontSize::Large, 1.0, 0.5, KDColorBlack)
{
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
}

void RpnPromptView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), Palette::GreyWhite);
}

void RpnPromptView::setText(const char *text) {
  m_bufferTextView.setText(text);
}

int RpnPromptView::numberOfSubviews() const {
  return 1;
}

View * RpnPromptView::subviewAtIndex(int index) {
  return &m_bufferTextView;
}

void RpnPromptView::layoutSubviews() {
  m_bufferTextView.setFrame(KDRect(k_border, k_border, bounds().width()-2*k_border, bounds().height()-2*k_border));
}

}
