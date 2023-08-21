netconfmodule_LTLIBRARIES = liblsi-display.la

liblsi_display_la_SOURCES = \
mod.c \
dict.c \
decode.c

# Depends on installed libyuma-dev
liblsi_display_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_display_la_LDFLAGS = -module -lyumaagt -lyumancx -ljpeg

yang_DATA = lsi-display.yang

bin_PROGRAMS = display-pipe

display_pipe_SOURCES = \
display.c

display_pipe_CPPFLAGS =
display_pipe_LDFLAGS = -lX11

#bin_SCRIPTS = display-pipe-wrapper

