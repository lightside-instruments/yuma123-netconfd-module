netconfmodule_LTLIBRARIES = liblsi-bar.la

liblsi_bar_la_SOURCES = \
lsi-bar.c

# Depends on installed libyuma-dev
liblsi_bar_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_bar_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-bar.yang

bin_SCRIPTS = lsi-bar-set
