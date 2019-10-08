TOP_DIR     :=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
TOP_DIR     :=$(shell dirname $(TOP_DIR))

CC          = $(CROSS_COMPILE)gcc
CXX         = $(CROSS_COMPILE)g++
STRIP       = $(CROSS_COMPILE)strip
MAKE        ?= make
RM          ?= rm -rf
INSTALL     ?= install -c
PREFIX      ?= /usr/local

DEFFLAGS    = -g -pthread -I$(TOP_DIR) -ffunction-sections -fdata-sections -fPIC
CFLAGS     += -D DEBUG_LOG
CFLAGS      += $(DEFFLAGS)
CXXFLAGS     += -D DEBUG_LOG
CXXFLAGS    += $(DEFFLAGS) -std=c++11

PROTOC      ?= `which protoc`

LDFLAGS     += -pthread -lprotobuf-lite -lmosquittopp -lparson
LDFLAGS     += -Wl,--gc-sections --shared
LDFLAGS     += -Wl,--gc-sections -Wl,-wrap,malloc -Wl,-wrap,free \
               -Wl,-wrap,calloc -Wl,-wrap,realloc

LDFLAGS     += $(EXTRA_LDFLAGS)
CFLAGS      += $(EXTRA_CFLAGS)
CXXFLAGS    += $(EXTRA_CXXFLAGS)

PBS:= he.proto

GEN_HDRS    := $(patsubst %.proto,%.pb.h,$(PBS))
GEN_SRCS    := $(patsubst %.proto,%.pb.cc,$(PBS))
GENS        := $(GEN_HDRS) $(GEN_SRCS)

SRCS:= \
    memory.c \
    timer.cc \
    thing.cc \
    mqtt.cc \
    uuid.cc \
    mqtt_client.cc \
    device_agent.cc \
    link_agent.cc \
    gateway_client.cc \
    crc8.c \

HDRS:= \
    $(GEN_HDRS) \
    link_agent.hh \
    device_agent.hh \
    device_agent_io_lock.hh \
    device_agent_pdu.hh \
    device_agent_ipc.hh \
    device_agent_env_monitor.hh \
    device_agent_info_screen.hh \
    device_agent_street_lamp.hh \
    device_agent_video_call.hh \
    device_agent_electric_meter.hh \
    gateway_client.hh \
    mqtt_client.hh \
    thing.hh \
    uuid.hh \
    mqtt.hh \
    timer.hh \
    types.h \
    memory.h \
    name_define.hh \

DEPS:=$(patsubst %.cc,%.dep,$(patsubst %.c,%.dep,$(SRCS)))
OBJS:=$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRCS) $(GEN_SRCS)))

LIB_SO      :=libmu2.so
LIB_A       :=libmu2.a

vpath %.proto

.PHONY : all clean install

.SECONDARY : $(GENS)

all: $(LIB_SO)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(LIB_SO): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@	

$(LIB_A): $(OBJS)
	$(AR) -crv $@ $(OBJS)

clean:
	$(RM) $(DEPS) $(OBJS) $(GENS) $(LIB_SO) $(LIB_A)

install: all
		$(INSTALL) -d $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include/mu2
		$(INSTALL) -s --strip-program=$(STRIP) $(LIB_SO) $(DESTDIR)$(PREFIX)/lib
		$(INSTALL) -m 644 $(HDRS) $(DESTDIR)$(PREFIX)/include/mu2

%.pb.cc %.pb.h: %.proto
	@echo "Generating $@ ..."
	@$(PROTOC) --cpp_out=. $<

%.dep: %.c $(GENS)
	@echo "Depending $< ..."
	@set -e; \
    $(RM) $@.tmp; \
    $(CC) $(CFLAGS) -E -MM $< > $@.tmp; \
    sed 's#^\(.*\.o\):#$(@D)/\1 $@: #g' < $@.tmp >$@; \
    $(RM) $@.tmp

%.dep: %.cc $(GENS)
	@echo "Depending $< ... "
	@set -e; \
    $(RM) $@.tmp; \
    $(CXX) $(CXXFLAGS) -E -MM $< > $@.tmp; \
    sed 's#^\(.*\.o\):#$(@D)/\1 $@: #g' < $@.tmp >$@; \
    $(RM) $@.tmp

%.o: %.c
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo "Compiling $< ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

