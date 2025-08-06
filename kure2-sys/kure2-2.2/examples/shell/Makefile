CC = gcc
CFLAGS = -g -Wall -O
KURE_CFLAGS = `pkg-config kure2 --cflags`
KURE_LIBS = `pkg-config kure2 --libs`

CFLAGS += $(KURE_CFLAGS)
LIBS += $(KURE_LIBS) -lreadline -ltermcap

OBJS = shell.o

.PHONY: all check_pkgconfig shell

all: check_pkgconfig shell

check_pkgconfig:
	@if test `pkg-config kure2; echo $$?` '=' "1"; then \
		echo "pkg-config was unable to find file kure2.pc. Maybe you forgot to set "; \
		echo "PKG_CONFIG_PATH accordingly to your prefix or to install the package using "; \
		echo "'make install'"; \
		exit 1; \
	 fi

shell: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o shell
