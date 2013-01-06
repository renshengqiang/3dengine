CC:=gcc
CXX:=g++
RM:=rm -rf

OUTDIR:=$(EXEDIR)
EXENAME:=$(PROJNAME)
EXESRCS:=$(COMPILE_FILES)
CFLAGS:=$(COMPILE_FLAGS)
LDFLAGS:=$(LINK_FLAGS)
DEBUG:=Y

EXEOBJS:=$(patsubst %.cpp,%.oo,$(EXESRCS))
EXEOBJS:=$(patsubst %.c,%.o,$(EXEOBJS))
EXEDEPS:=$(patsubst %.cpp,%.dd,$(EXESRCS))
EXEDEPS:=$(patsubst %.c,%.d,$(EXEDEPS))

ifeq ($(DEBUG),Y)
CFLAGS+=-O0 -ggdb3
else
CFLAGS+=-O3 -fomit-frame-pointer
endif

.PHONY:all clean
all:$(OUTDIR).dir $(OUTDIR)/$(EXENAME)
 
$(OUTDIR)/$(EXENAME):$(EXEDEPS) $(EXEOBJS) 
	$(CXX)  $(EXEOBJS) -o $@ $(LDFLAGS)


%.d: %.c
	@$(CC) -MM $(CFLAGS) $< | sed -e 's,^[0-9a-zA-Z._-]*: \([0-9a-zA-Z._-]*/\),\1&,' -e 's,^\([0-9a-zA-Z._-/]*\)\.o:,\1.o \1.d:,' > $@
%.dd: %.cpp
	@$(CXX) -MM $(CFLAGS) $< | sed -e 's,^[0-9a-zA-Z._-]*: \([0-9a-zA-Z._-]*/\),\1&,' -e 's,^\([0-9a-zA-Z._-/]*\)\.o:,\1.oo \1.dd:,' > $@
%.o:%.c
	$(CC) $(CFLAGS)  -c $< -o $@
%.oo:%.cpp
	$(CXX) $(CFLAGS)  -c $< -o $@
%.dir:
	@echo "Checking directory $*"
	@if [ ! -d $* ]; then \
		echo "Making directory $*"; \
		mkdir -p $* ;\
	fi;

clean:
	-$(RM) $(EXEOBJS) $(EXEDEPS) $(OUTDIR)/$(EXENAME)
