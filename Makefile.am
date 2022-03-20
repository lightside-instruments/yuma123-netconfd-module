netconfmodule_LTLIBRARIES = liblsi-foo.la

liblsi_foo_la_SOURCES = \
lsi-foo.c

# Depends on installed libyuma-dev
liblsi_foo_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_foo_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-foo.yang

bin_SCRIPTS = lsi-foo-get
