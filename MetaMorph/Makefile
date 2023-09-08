# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

include $(RACK_DIR)/arch.mk

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += 

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS += -L/Users/kodiak/projects/VCVModules/EigenLite -lpicodecoder 
# TODO: not working !
# LDFLAGS +=  -Wl,-rpath=.

EL_DIR=EL/eigenapi
CFLAGS += -I ${EL_DIR}
CXXFLAGS += -I ${EL_DIR} -I ${EL_DIR}/picross


EIGENHARP_SRC = \
    ${EL_DIR}/picross/src/pic_backtrace.c\
	${EL_DIR}/picross/src/pic_time.c\
	${EL_DIR}/picross/src/pic_usb_generic.cpp\
    ${EL_DIR}/picross/src/usb_serial.cpp\
	${EL_DIR}/picross/src/pic_safeq.cpp\
    ${EL_DIR}/picross/src/pic_error.cpp\
    ${EL_DIR}/picross/src/pic_log.cpp\
    ${EL_DIR}/picross/src/pic_fastalloc.cpp\
    ${EL_DIR}/picross/src/pic_mlock.cpp\
	${EL_DIR}/picross/src/pic_resources.cpp\
	${EL_DIR}/lib_alpha2/src/alpha2_passive.cpp\
    ${EL_DIR}/lib_alpha2/src/alpha2_active.cpp\
    ${EL_DIR}/lib_pico/src/pico_passive.cpp\
    ${EL_DIR}/lib_pico/src/pico_active.cpp\
    ${EL_DIR}/src/eigenlite.cpp\
    ${EL_DIR}/src/ef_harp.cpp\
    ${EL_DIR}/src/ef_basestation.cpp\
    ${EL_DIR}/src/ef_alpha.cpp\
    ${EL_DIR}/src/ef_tau.cpp\
    ${EL_DIR}/src/ef_pico.cpp\
    ${EL_DIR}/src/eigenapi.cpp\
    ${EL_DIR}/src/fwr_posix.cpp

ifdef ARCH_X64
endif
ifdef ARCH_ARM64
endif

ifdef ARCH_LIN
EIGENHARP_SRC += \
        ${EL_DIR}/picross/src/pic_thread_posix.cpp\
        ${EL_DIR}/picross/src/pic_usb_linux.cpp\
        ${EL_DIR}/picross/src/pic_tool_linux.cpp
endif
ifdef ARCH_MAC
EIGENHARP_SRC += \
    ${EL_DIR}/picross/src/pic_thread_posix.cpp\
    ${EL_DIR}/picross/src/pic_cfrunloop.cpp\
    ${EL_DIR}/picross/src/pic_usb_macosx.cpp\
	${EL_DIR}/picross/src/pic_tool_macosx.cpp

#TODO: is this needed? I think not! 
#    ${EL_DIR}/picross/src/pic_power_macosx.mm 

endif
ifdef ARCH_WIN
EIGENHARP_SRC += \
        ${EL_DIR}/picross/src/pic_thread_win32.cpp\
        ${EL_DIR}/picross/src/pic_usb_win32.cpp\
        ${EL_DIR}/picross/src/pic_winloop.cpp\
        ${EL_DIR}/picross/src/pic_tool_win32.cpp
endif

# Add .cpp files to the build
SOURCES += ${EIGENHARP_SRC}
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
