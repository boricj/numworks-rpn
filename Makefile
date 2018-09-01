snapshots += Rpn::App::Snapshot
snapshot_headers += apps/rpn/app.h

app_objs += $(addprefix apps/rpn/,\
  app.o\
  rpn_prompt_controller.o\
  rpn_stack_controller.o\
  rpn_stack.o\
)

i18n_files += $(addprefix apps/rpn/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

app_images += apps/rpn/rpn_icon.png
