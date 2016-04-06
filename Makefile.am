AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I build

AM_CFLAGS  =
AM_CFLAGS += -Wall
AM_CFLAGS += -Wextra
AM_CFLAGS += -g
AM_CFLAGS += -pedantic
AM_CFLAGS += -std=gnu99

CORE_SRC  =
CORE_SRC += src/proccer.h src/proccer.c

bin_PROGRAMS          = proccer
proccer_DEPENDENCIES = config.h
proccer_SOURCES      = $(CORE_SRC) src/main.c


test: check
