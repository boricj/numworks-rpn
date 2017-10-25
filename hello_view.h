#ifndef HELLOWORLD_HELLOVIEW_H
#define HELLOWORLD_HELLOVIEW_H

#include <escher.h>

namespace Helloworld {

class HelloView : public View {
public:
  HelloView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  void changeColor();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews() override;
  BufferTextView m_bufferTextView;
  int m_color;
  KDColor m_kdcolor;
};

}

#endif
