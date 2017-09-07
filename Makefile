ifneq (,$(findstring boricj.hello-world,${APPS_LIST}))

app_objs += $(addprefix apps/boricj.hello-world/,\
  app.o\
  hello_controller.o\
  hello_view.o\
)

app_images += apps/boricj.hello-world/helloworld_icon.png

endif
