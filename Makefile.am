netconfmodule_LTLIBRARIES = liblsi-ivi-scope.la

liblsi_ivi_scope_la_SOURCES = \
mod.c

# Depends on installed libyuma-dev
liblsi_ivi_scope_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_ivi_scope_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-ivi-scope.yang

#bin_SCRIPTS = lsi-ivi-scope-set lsi-ivi-scope-gpib-e364xa-set lsi-ivi-scope-get lsi-ivi-scope-gpib-e364xa-get

