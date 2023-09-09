sudo gdb -ex "attach $(pidof hl2_linux)"  \
  -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
  -ex "set \$dlclose = (int(*)(void*)) dlclose" \
  -ex "set \$library = \$dlopen(\"/home/bob/Coding/cheats/mystuff/upimgui/libsdl-imgui.so\", 1)" \
  -ex "b ImGui::ErrorCheckEndFrameSanityChecks" \
  -ex "b assert" \

call $dlclose($library)
set $library = $dlopen("/home/bob/Coding/cheats/mystuff/upimgui/libsdl-imgui.so", 1)
