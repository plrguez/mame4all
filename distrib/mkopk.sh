rm -f mame4all.opk
rm -f mame4all_ui_only.opk

mksquashfs mame4all mame4all.opk -all-root -noappend -no-exports -no-xattrs -force-uid 1000 -force-gid 100
mksquashfs mame4all mame4all_ui_only.opk -e mame4all_explorer.gcw0.desktop -e aliases.txt -all-root -noappend -no-exports -no-xattrs -force-uid 1000 -force-gid 100
