include doxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST = $(DX_CONFIG)

install-data-hook:
	cp -r $(DX_DOCDIR)/html $(htmldir)
	cp $(DX_DOCDIR)/SmartGridToolbox.pdf $(pdfdir)

all-local: doxygen-doc
