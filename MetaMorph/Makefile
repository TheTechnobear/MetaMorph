# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

include $(RACK_DIR)/arch.mk

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += 
LDFLAGS += 

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.

EL_DIR=EigenLite/eigenapi
EL_RES_DIR=EigenLite/resources
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
    ${EL_DIR}/src/fwr_posix.cpp\
    ${EL_DIR}/src/fwr_embedded.cpp
 

EMB_FW_SRC_DIR=${EL_RES_DIR}/firmware/cpp

EIGENHARP_SRC += \
    ${EMB_FW_SRC_DIR}/bs_mm_fw_0103.cpp\
    ${EMB_FW_SRC_DIR}/pico.cpp\
    ${EMB_FW_SRC_DIR}/psu_mm_fw_0102.cpp	


ifdef ARCH_X64
endif
ifdef ARCH_ARM64
endif

ifdef ARCH_LIN
EIGENHARP_SRC += \
        ${EL_DIR}/picross/src/pic_thread_posix.cpp\
        ${EL_DIR}/picross/src/pic_usb_linux.cpp\
        ${EL_DIR}/picross/src/pic_tool_linux.cpp
        
LDFLAGS += ${EL_RES_DIR}/picodecoder/linux/x86_64/libpicodecoder_static.a

endif
ifdef ARCH_MAC
EIGENHARP_SRC += \
    ${EL_DIR}/picross/src/pic_thread_posix.cpp\
    ${EL_DIR}/picross/src/pic_cfrunloop.cpp\
    ${EL_DIR}/picross/src/pic_usb_macosx.cpp\
	${EL_DIR}/picross/src/pic_tool_macosx.cpp

#    ${EL_DIR}/picross/src/pic_power_macosx.mm 

ifdef ARCH_X64
LDFLAGS += ${EL_RES_DIR}/picodecoder/macOs/x86_64/libpicodecoder_static.a
endif
ifdef ARCH_ARM64
LDFLAGS += ${EL_RES_DIR}/picodecoder/macOs/arm64/libpicodecoder_static.a
endif


endif
ifdef ARCH_WIN
EIGENHARP_SRC += \
        ${EL_DIR}/picross/src/pic_thread_win32.cpp\
        ${EL_DIR}/picross/src/pic_usb_win32.cpp\
        ${EL_DIR}/picross/src/pic_winloop.cpp\
        ${EL_DIR}/picross/src/pic_tool_win32.cpp
endif

# libscala-file

LS_DIR = libscala-file/src
CFLAGS += -I ${LS_DIR}
CXXFLAGS += -I ${LS_DIR}

LS_SRC = \
    ${LS_DIR}/scala_scl.cpp\
    ${LS_DIR}/scala_kbm.cpp


# Add .cpp files to the build
SOURCES += ${EIGENHARP_SRC}
SOURCES += ${LS_SRC}
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
