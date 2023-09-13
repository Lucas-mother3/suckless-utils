install:
	mkdir -p ${PREFIX}${DESTDIR}/bin
	cp -f volume-dunst ${PREFIX}${DESTDIR}/bin
	chmod 755 ${PREFIX}${DESTDIR}/bin/volume-dunst
	cp -f notify-vol-send ${PREFIX}${DESTDIR}/bin/notify-vol-send
	chmod 755 ${PREFIX}${DESTDIR}/bin/notify-vol-send

uninstall:
	        rm -f ${PREFIX}${DESTDIR}/bin/volume-dunst
		rm -f ${PREFIX}${DESTDIR}/bin/notify-vol-send
