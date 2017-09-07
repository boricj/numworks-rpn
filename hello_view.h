#ifndef BORICJ_HELLOWORLD_HELLOVIEW_H
#define BORICJ_HELLOWORLD_HELLOVIEW_H

#include <escher.h>

namespace boricj {
namespace helloworld {

class HelloView : public View {
public:
  HelloView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  void changeColor();
private:
  int m_color;
};

}
}

#endif
