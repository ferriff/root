# Module.mk for VDT
#
# Author: Danilo Piparo

MODNAME       := vdt
MODDIR        := $(ROOT_SRCDIR)/math/$(MODNAME)
MODDIRI       := $(MODDIR)/include
MODDIRS       := $(MODDIR)/include

VDTH    := include/vdt/asin.h\
	include/vdt/atan2.h\
	include/vdt/atan.h\
	include/vdt/cos.h\
	include/vdt/exp.h\
	include/vdt/inv.h\
	include/vdt/log.h\
	include/vdt/sincos.h\
	include/vdt/sin.h\
	include/vdt/sqrt.h\
	include/vdt/tan.h\
	include/vdt/vdtcore_common.h\
	include/vdt/vdtMath.h

# We create a stamp file as vdt is the only module without a library.
VDTSTAMP := vdt.stamp

ALLLIBS      += $(VDTSTAMP)

##### local rules #####
.PHONY:         all-$(MODNAME) clean-$(MODNAME) distclean-$(MODNAME) \
                test-$(MODNAME)

include/vdt/%.h: $(MODDIRI)/vdt/%.h
		@(if [ ! -d "include/vdt" ]; then   \
		mkdir -p include/vdt;       \
		fi)
		cp $< $@

$(VDTSTAMP): $(VDTH)
	@touch $(MODDIR)/$(VDTSTAMP)

all-$(MODNAME): $(VDTSTAMP)
	$(noop)

clean-$(MODNAME):
	@rm $(MODDIR)/$(VDTSTAMP)

clean:: clean-$(MODNAME)
	$(noop)

distclean-$(MODNAME): clean-$(MODNAME)
	$(noop)

distclean:: distclean-$(MODNAME)
