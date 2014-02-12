if HAVE_DOXYGEN

install-data-hook:
	cp -r doxygen-doc/html $(htmldir)

doxygen-doc/html/index.html: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -rf doxygen-doc

endif
