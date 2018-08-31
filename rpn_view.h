#ifndef RPN_RPNVIEW_H
#define RPN_RPNVIEW_H

#include <escher.h>

namespace Rpn {

class RpnView : public View {
public:
  RpnView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews() override;
  BufferTextView m_bufferTextView;
};

}

#endif
