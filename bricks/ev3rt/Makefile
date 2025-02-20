MPTOP = ../../../..
CROSS_COMPILE=arm-none-eabi-

# NXT:
# CROSS_COMPILE=/path/to/bin/arm-elf-
-include mpconfigport.mk
include $(MPTOP)/py/mkenv.mk

# ensure required git submodules checked out
ifeq ("$(wildcard $(TOP)/ports/pybricks/lib/libfixmath/README.md)","")
$(info GIT cloning libfixmath submodule)
$(shell cd $(TOP)/ports/pybricks && git submodule update --init lib/libfixmath)
endif
ifeq ("$(wildcard $(TOP)/ports/pybricks/bricks/ev3rt/ev3rt-hrp2/README.md)","")
$(info GIT cloning ev3rt-hrp2 submodules)
$(shell cd $(TOP)/ports/pybricks && git submodule update --checkout --init --recursive bricks/ev3rt/ev3rt-hrp2)
endif

all: lib

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(MPTOP)/py/py.mk

INC +=  -I.
INC +=  -I..
INC +=  -I$(MPTOP)
INC +=  -I$(MPTOP)/ports/unix
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)
# NXT:
# CFLAGS += -DMP_ENDIANNESS_LITTLE -mthumb-interwork

# Compiler flags should match TOPPERS kernel
CFLAGS += -mcpu=arm926ej-s -mlittle-endian

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
# _FORTIFY_SOURCE is a feature in gcc/glibc which is intended to provide extra
# security for detecting buffer overflows. Some distros (Ubuntu at the very least)
# have it enabled by default.
#
# gcc already optimizes some printf calls to call puts and/or putchar. When
# _FORTIFY_SOURCE is enabled and compiling with -O1 or greater, then some
# printf calls will also be optimized to call __printf_chk (in glibc). Any
# printfs which get redirected to __printf_chk are then no longer synchronized
# with printfs that go through mp_printf.
#
# In MicroPython, we don't want to use the runtime library's printf but rather
# go through mp_printf, so that stdout is properly tied into streams, etc.
# This means that we either need to turn off _FORTIFY_SOURCE or provide our
# own implementation of __printf_chk. We've chosen to turn off _FORTIFY_SOURCE.
# It should also be noted that the use of printf in MicroPython is typically
# quite limited anyways (primarily for debug and some error reporting, etc
# in the unix version).
#
# Information about _FORTIFY_SOURCE seems to be rather scarce. The best I could
# find was this: https://securityblog.redhat.com/2014/03/26/fortify-and-you/
# Original patchset was introduced by
# https://gcc.gnu.org/ml/gcc-patches/2004-09/msg02055.html .
#
# Turning off _FORTIFY_SOURCE is only required when compiling with -O1 or greater
CFLAGS += -U _FORTIFY_SOURCE
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
CC = clang
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref
endif
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

ifeq ($(MICROPY_FORCE_32BIT),1)
# Note: you may need to install i386 versions of dependency packages,
# starting with linux-libc-dev:i386
ifeq ($(MICROPY_PY_FFI),1)
ifeq ($(UNAME_S),Linux)
CFLAGS_MOD += -I/usr/include/i686-linux-gnu
endif
endif
endif

ifeq ($(MICROPY_USE_READLINE),1)
INC +=  -I$(MPTOP)/shared/readline
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
SHARED_SRC_C_EXTRA += readline/readline.c
endif
ifeq ($(MICROPY_USE_READLINE),2)
CFLAGS_MOD += -DMICROPY_USE_READLINE=2
LDFLAGS_MOD += -lreadline
# the following is needed for BSD
#LDFLAGS_MOD += -ltermcap
endif
ifeq ($(MICROPY_PY_TIME),1)
CFLAGS_MOD += -DMICROPY_PY_TIME=1
SRC_MOD += modtime.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += modsocket.c
endif

ifeq ($(MICROPY_PY_FFI),1)

ifeq ($(MICROPY_STANDALONE),1)
LIBFFI_CFLAGS_MOD := -I$(shell ls -1d $(MPTOP)/lib/libffi/build_dir/out/lib/libffi-*/include)
 ifeq ($(MICROPY_FORCE_32BIT),1)
  LIBFFI_LDFLAGS_MOD = $(MPTOP)/lib/libffi/build_dir/out/lib32/libffi.a
 else
  LIBFFI_LDFLAGS_MOD = $(MPTOP)/lib/libffi/build_dir/out/lib/libffi.a
 endif
else
LIBFFI_CFLAGS_MOD := $(shell pkg-config --cflags libffi)
LIBFFI_LDFLAGS_MOD := $(shell pkg-config --libs libffi)
endif

ifeq ($(UNAME_S),Linux)
LIBFFI_LDFLAGS_MOD += -ldl
endif

CFLAGS_MOD += $(LIBFFI_CFLAGS_MOD) -DMICROPY_PY_FFI=1
LDFLAGS_MOD += $(LIBFFI_LDFLAGS_MOD)
SRC_MOD += modffi.c
endif

# source files
SRC_C = $(addprefix ports/unix/,\
	gccollect.c \
	input.c \
	file.c \
	modmachine.c \
	moduselect.c \
	coverage.c \
	fatfs_port.c \
	$(SRC_MOD) \
	) \
	$(addprefix ports/pybricks/bricks/ev3rt/,\
	mphal.c \
	)

SHARED_SRC_C = $(addprefix shared/,\
	runtime/printf.c \
	runtime/interrupt_char.c \
	timeutils/timeutils.c \
	$(SHARED_SRC_C_EXTRA) \
	)

ifeq ($(MICROPY_FATFS),1)
LIB_SRC_C += $(addprefix lib/,\
	fatfs/ff.c \
	fatfs/option/ccsbcs.c \
	)
endif

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(LIB_SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

include $(MPTOP)/py/mkrules.mk

# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overridden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

deplibs: libffi axtls

# install-exec-recursive & install-data-am targets are used to avoid building
# docs and depending on makeinfo
libffi:
	cd $(MPTOP)/lib/libffi; git clean -d -x -f
	cd $(MPTOP)/lib/libffi; ./autogen.sh
	mkdir -p $(MPTOP)/lib/libffi/build_dir; cd $(MPTOP)/lib/libffi/build_dir; \
	../configure $(CROSS_COMPILE_HOST) --prefix=$$PWD/out CC="$(CC)" CXX="$(CXX)" LD="$(LD)"; \
	make install-exec-recursive; make -C include install-data-am

axtls: $(MPTOP)/lib/axtls/README
	cd $(MPTOP)/lib/axtls; cp config/upyconfig config/.config
	cd $(MPTOP)/lib/axtls; make oldconfig -B
	cd $(MPTOP)/lib/axtls; make clean
	cd $(MPTOP)/lib/axtls; make all CC="$(CC)" LD="$(LD)"

$(MPTOP)/lib/axtls/README:
	@echo "You cloned without --recursive, fetching submodules for you."
	(cd $(MPTOP); git submodule update --init --recursive)
