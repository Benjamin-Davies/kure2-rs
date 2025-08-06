CC = gcc-svn
CXX = g++-svn
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall
KURE_CFLAGS = `pkg-config kure2 --cflags`
KURE_LIBS = `pkg-config kure2 --libs` -ldl

EXAMPLES = basics dsl cxx dsl_to_lua

.PHONY: all check_pkgconfig

all: check_pkgconfig $(EXAMPLES)

check_pkgconfig:
	@if test `pkg-config kure2; echo $$?` '=' "1"; then \
		echo "pkg-config was unable to find file kure2.pc. Maybe you forgot to set "; \
		echo "PKG_CONFIG_PATH accordingly to your prefix or to install the package using "; \
		echo "'make install'"; \
		exit 1; \
	 fi

basics: basics.c
	$(CC) basics.c $(CFLAGS) $(KURE_CFLAGS) $(KURE_LIBS) -o $@

dsl: dsl.c
	$(CC) dsl.c $(CFLAGS) $(KURE_CFLAGS) $(KURE_LIBS) -o $@

dsl_to_lua: dsl_to_lua.c
	$(CC) dsl_to_lua.c $(CFLAGS) $(KURE_CFLAGS) $(KURE_LIBS) -o $@

cxx:
