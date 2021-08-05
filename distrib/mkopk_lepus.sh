rm -f mame4all_lepus.opk
rm -f mame4all_ui_only_lepus.opk

cp ./mame4all/mame4all_explorer.gcw0.desktop ./mame4all/mame4all_explorer.lepus.desktop
cp ./mame4all/mame4all_ux.gcw0.desktop ./mame4all/mame4all_ux.lepus.desktop
mksquashfs mame4all mame4all_lepus.opk -e mame4all_explorer.gcw0.desktop -e mame4all_ux.gcw0.desktop -all-root -no-xattrs -noappend -no-exports
mksquashfs mame4all mame4all_ui_only_lepus.opk -e mame4all_explorer.gcw0.desktop -e mame4all_ux.gcw0.desktop -e mame4all_explorer.lepus.desktop -e aliases.txt -all-root -no-xattrs -noappend -no-exports
rm ./mame4all/mame4all_explorer.lepus.desktop
rm ./mame4all/mame4all_ux.lepus.desktop