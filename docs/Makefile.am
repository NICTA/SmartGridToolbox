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
	rm -rf doxygen-doc doxyfile.stamp

if HAVE_PDFLATEX

PDF_DOC_DIR = doxygen-doc/latex
PDF_DOC = $(PDF_DOC_DIR)/SmartGridToolbox.pdf

pdf_DATA = $(PDF_DOC)

$(PDF_DOC): doxyfile.stamp
	cd $(PDF_DOC_DIR); \
	$(PDFLATEX) refman.tex;\
	$(MAKEINDEX) refman.tex;\
	$(PDFLATEX) refman.tex; \
	cp refman.pdf SmartGridToolbox.pdf

endif

endif
