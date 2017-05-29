
# The directory where the data files will be installed

install-data-local:
	if [ ! -d $(prefix)/etc ]; then $(INSTALL) -d $(prefix)/etc; fi

	$(INSTALL_DATA) mgncs.cfg $(prefix)/etc/mgncs.cfg

EXTRA_DIST = mgncs.cfg

