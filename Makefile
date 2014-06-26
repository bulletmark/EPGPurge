#### Edit BELOW values appropriately for your project

# Name of TAP
APP = EPGPurge

# C or C++ Source files that comprise TAP
SRCS = main id

# Documentation source file (in markdown plain-text format)
DOC = README.md

# C include file defining version number
VERSION = id.h

# Path to TMS TAP API LIB
TAPLIB = ../../tapapi

# Path to TMS cross-tools (i.e. mips compiler, linker, includes, etc)
XTOOLS = ../../xtools

# Path to Firebird library
FBLIB  = ../../FireBirdLib

# Target site where the zip and README file are sited
TSITE = bullet:/usr/share/nginx/markb/taps/$(APP)/

#### Edit ABOVE values appropriately for your project

XPATH = ${XTOOLS}/bin
JB = ${FBLIB}/devutils/JailBreak
OBJS = ${SRCS:=.o}
DOCOBJ = ${DOC:.md=.html}
ZIPDIR = $(APP)

# include default settings
include ${TAPLIB}/include/tool.mk

TAP_FLAGS += -fPIC

ifeq (${DEBUG},y)
TAP_FLAGS += -g -DDEBUG
endif

#compile options
LCFLAGS = -DLINUX -W -Wall -O2 -mips32 -MD
CFLAGS += $(LCFLAGS) -DFB_NO_TAP_PRINT_OVERRIDE
CPPFLAGS += $(LCFLAGS) -DFB_NO_TAP_PRINT_OVERRIDE

#include directories.
INCLUDE_DIRS = ${TAPLIB}/include ${FBLIB}

TAP_INCLUDE_DIRS = $(addprefix -I, $(INCLUDE_DIRS))
TAP_FLAGS += $(TAP_INCLUDE_DIRS)

TAP_OBJS = ${OBJS}

TAP_LIBS_CPP = -lstdc++ -lgcc_s
TAP_LIBS = ${FBLIB}/libFireBird.a ${TAPLIB}/tapinit.o ${TAPLIB}/libtap.so -lc -ldl ${TAP_LIBS_CPP}

TAP_APP = $(APP).tap
TAP_MAP = $(APP).map

ALL = $(TAP_APP) $(DOCOBJ)

all: $(ALL)

$(TAP_APP): ${TAP_OBJS}
	@echo "[Linking .. $@]"
	$(Q_)${XPATH}/$(LD) -shared --no-undefined --allow-shlib-undefined -o $@ ${TAP_OBJS} $(TAP_LIBS) -Map ${TAP_MAP}
	$(Q_)${JB} $@

$(DOCOBJ): $(DOC)
	markdown $< >$@

# Implicit rule for building local apps
%.o : %.c
	@echo [Compiling .. $<]
	${Q_}${XPATH}/$(CC) $(CFLAGS) $(TAP_FLAGS) -c $< -o $@

%.o : %.cc
	@echo "[Compiling .. $<]"
	${Q_}${XPATH}/$(CXX) $(CPPFLAGS) $(TAP_FLAGS) -c $< -o $@

version = $(shell sed -n \
	  '/define[ \t]*VERSION[ \t]/s/^[^"]*"\(.*\)".*$$/\1/p' \
	  $(VERSION))

ZIPNAME = $(ZIPDIR)-$(version)
ZIPFILE = $(APP)-$(version).zip
ZIPLINK = $(APP).zip

zip:	$(ZIPFILE)

$(ZIPFILE): $(ALL)
	rm -rf $(ZIPNAME)
	mkdir $(ZIPNAME)
	cp -p $(TAP_APP) $(DOC) $(DOCOBJ) $(ZIPNAME)
	zip -r $(ZIPFILE) $(ZIPNAME)
	rm -rf $(ZIPNAME)
	ln -sf $(ZIPFILE) $(ZIPLINK)

site:	zip $(DOCOBJ)
	rsync -av $(ZIPFILE) $(ZIPLINK) $(DOCOBJ) $(TSITE)

clean:
	@echo "[Cleaning all .. ]"
	$(Q_)-${RM} -rf $(TAP_APP) $(TAP_MAP) *.d *.o $(DOCOBJ) $(ZIPDIR)-*/ $(APP)-*.zip $(ZIPLINK)

# Dependency file checking (created with gcc -M command)
-include *.d
