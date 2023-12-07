netconfmodule_LTLIBRARIES = liblsi-ivi-switch.la

liblsi_ivi_switch_la_SOURCES = \
lsi-ivi-switch.c \
relay-actuator-io-system.c

#relay-actuator-io-system.c - calling external python executable script. Note on Pi4B it takes 40 ms to call python script with system(), 2 ms for native program, 4ms for bash script
#relay-actuator-io-spark.c - faster version that does not need to start external executable

# Depends on installed libyuma-dev
liblsi_ivi_switch_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS) -DUSE_GPIB
liblsi_ivi_switch_la_LDFLAGS = -module -lyumaagt -lyumancx -lgpib


yang_DATA = lsi-ivi-switch.yang

bin_SCRIPTS = lsi-ivi-switch-set lsi-ivi-switch-gpib-set lsi-ivi-switch-spark-set
