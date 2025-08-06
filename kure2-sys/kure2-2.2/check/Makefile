CC = gcc
CXX = g++
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall
KURE_CFLAGS = `pkg-config kure2 --cflags`
KURE_LIBS = `pkg-config kure2 --libs`

CFLAGS += $(KURE_CFLAGS)
LIBS += $(KURE_LIBS) -ldl

OBJS = Mat.o main.o

.PHONY: all check_pkgconfig check

all: check_pkgconfig check

check_pkgconfig:
	@if test `pkg-config kure2; echo $$?` '=' "1"; then \
		echo "pkg-config was unable to find file kure2.pc. Maybe you forgot to set "; \
		echo "PKG_CONFIG_PATH accordingly to your prefix or to install the package using "; \
		echo "'make install'"; \
		exit 1; \
	 fi

check: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o check
