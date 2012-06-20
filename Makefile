# makefile

# architecture
TARGET_ARCH = linux

# compiler
CC     = g++ 

# debug flag
DEBUG  = -g2
OTHER  = -Wall
LDFLAG = -rdynamic

#CFLAGS = $(DEBUG) $(OTHER) $(LDFLAG)
CFLAGS = $(DEBUG) $(LDFLAG)

MODULE = nirgam

ROUTER_MODEL = core/orion/SIM_router.cpp core/orion/SIM_arbiter.cpp core/orion/SIM_crossbar.cpp 
POWER_MODEL	 = core/orion/SIM_router_power.cpp core/orion/orion_router_power.cpp
LINK_MODEL   = core/orion/SIM_link.cpp
CLOCK_MODEL  = core/orion/SIM_clock.cpp
AREA_MODEL   = core/orion/orion_router_area.cpp core/orion/SIM_router_area.cpp
ARRAY_MODEL  = core/orion/SIM_array_l.cpp core/orion/SIM_array_m.cpp core/orion/SIM_cam.cpp
ALU_MODEL    = core/orion/SIM_ALU.cpp
OTHER_MODEL  = core/orion/SIM_misc.cpp core/orion/SIM_permu.cpp
STATIC_MODEL = core/orion/SIM_static.cpp
MISC_SRC     = core/orion/SIM_util.cpp core/orion/SIM_time.cpp

SRCS         = $(ARRAY_MODEL) $(ROUTER_MODEL) $(POWER_MODEL) $(ALU_MODEL) $(OTHER_MODEL) \
               $(STATIC_MODEL) $(LINK_MODEL) $(CLOCK_MODEL) $(MISC_SRC) $(AREA_MODEL)

CORE_SRCS = \
	core/rng.cpp \
	core/fifo.cpp \
	core/InputChannel.cpp \
	core/ipcore.cpp \
	core/OutputChannel.cpp \
	core/VCAllocator.cpp \
	core/NWTile.cpp \
	core/NoC.cpp \
	$(SRCS) \
	core/main.cpp \
	core/Controller.cpp \
	core/ranvar.cpp \
	application/src/TG.cpp\
	application/src/Traffic_patterns.cpp \
	application/src/NED_traffic.cpp

APP_SRCS = \
	application/src/App_send.cpp \
	application/src/App_concat.cpp \
	application/src/App_recv.cpp \
	application/src/CBR.cpp \
	application/src/Bursty.cpp \
	application/src/VBR.cpp\
	application/src/Trace_traffic.cpp \
	application/src/Sink.cpp \
	application/src/Multimedia.cpp \


ROUTER_SRCS = \
	router/src/OE_router.cpp \
	router/src/XY_router.cpp \
	router/src/source_router.cpp \
	router/src/Q_router.cpp\
	router/src/DYADrouter.cpp \
	router/src/maxy_router.cpp\
	router/src/ftxy.cpp\
	router/src/PROM_router.cpp\
	router/src/ER_router.cpp

CORE_OBJS = $(CORE_SRCS:.cpp=.o)

APP_OBJS = $(APP_SRCS:.cpp=.o)

ROUTER_OBJS = $(ROUTER_SRCS:.cpp=.o)

APP_LIB = $(subst src,lib,$(APP_OBJS:.o=.so))

ROUTER_LIB = $(subst src,lib,$(ROUTER_OBJS:.o=.so))

include Makefile.defs
# DO NOT DELETE
