#ifndef RPN_RPNVIEW_H
#define RPN_RPNVIEW_H

#include <escher.h>

namespace Rpn {

class RpnPromptView : public View {
public:
  RpnPromptView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setText(const char *text);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews() override;
  static constexpr int k_border = 8;
  BufferTextView m_bufferTextView;
};

}

#endif
