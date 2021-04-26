KVER:=$(shell uname -r)
KDIR:=/lib/modules/$(KVER)/build
KMISC:=/lib/modules/$(KVER)/kernel/drivers/net/
KFLAG:= 2$(shell echo $(KVER) | sed -ne 's/^2\.[4]\..*/4/p')x

ifneq ($(KFLAG),24x)
include $(PWD)/Makefile_linux26x
else
include $(PWD)/Makefile_linux24x
endif

default:
ifeq ($(KFLAG),24x)
	$(MAKE) -f Makefile_linux24x
else
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif

clear:
	rm -f dm9dev9000c.mod.*
	rm -f dm9dev9000c.*o
	rm -f .dm9dev9000c.*
	rm -rf .tmp_versions
