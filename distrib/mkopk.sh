rm -f mame4all.opk
rm -f mame4all_ui_only.opk

mksquashfs mame4all mame4all.opk
mksquashfs mame4all mame4all_ui_only.opk -e mame4all_explorer.gcw0.desktop -e aliases.txt
