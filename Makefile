snapshots += Helloworld::App::Snapshot
snapshot_headers += apps/hello_world/app.h

app_objs += $(addprefix apps/hello_world/,\
  app.o\
  hello_controller.o\
  hello_view.o\
)

i18n_files += $(addprefix apps/hello_world/,\
  base.de.i18n\
  base.en.i18n\
  base.es.i18n\
  base.fr.i18n\
  base.pt.i18n\
)

app_images += apps/hello_world/helloworld_icon.png
