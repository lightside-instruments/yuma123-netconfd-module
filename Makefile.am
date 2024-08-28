netconfmodule_LTLIBRARIES = liblsi-ivi-function-generator.la

liblsi_ivi_function_generator_la_SOURCES = \
mod.c

# Depends on installed libyuma-dev
liblsi_ivi_function_generator_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_ivi_function_generator_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-ivi-function-generator.yang

bin_SCRIPTS = lsi-ivi-function-generator-set

