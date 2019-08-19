apps += Rpn::App
app_headers += apps/rpn/app.h

app_src += $(addprefix apps/rpn/,\
  app.cpp\
  rpn_prompt_controller.cpp\
  rpn_stack_controller.cpp\
  rpn_stack.cpp\
)

i18n_files += $(addprefix apps/rpn/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

$(eval $(call depends_on_image,apps/rpn/app.cpp,apps/rpn/rpn_icon.png))

