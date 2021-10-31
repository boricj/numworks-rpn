apps += Rpn::App
app_headers += apps/rpn/app.h

apps_src += $(addprefix apps/rpn/,\
  app.cpp\
  rpn_content_view.cpp\
  rpn_input_controller.cpp\
  rpn_stack_controller.cpp\
  rpn_stack.cpp\
  rpn_toolbox.cpp\
)

i18n_files += $(call i18n_with_universal_for,rpn/base)

$(eval $(call depends_on_image,apps/rpn/app.cpp,apps/rpn/rpn_icon.png))

