
Dir := dummy
ALL_DIRS := $(sort $(dir $(wildcard */)))

CHROME := /mnt/c/Program\ Files/Google/Chrome/Application/chrome.exe

.PHONY: dir rm clean

all: dir

dir:
ifneq ($(Dir), dummy)
	mkdir -p $(Dir)
	cp template.mk $(Dir)/Makefile
	cp template.cpp $(Dir)/main.cpp
else
	@echo "Usage: make dir Dir=*"
endif

rm:
ifneq ($(Dir), dummy)
	-@rm -r $(Dir)
else
	@echo "Usage: make rm Dir=*"
endif

clean:
	@for d in $(ALL_DIRS); do \
		${MAKE} -C $${d} clean; \
	done

help:
	@echo "Usage: make Dir=*"

