CXX?=clang++
CC=$(CXX)
COMMON_CFLAGS?=-O0 -Wall -DDEBUG -URELEASE -fPIC -ggdb
export
all:
	$(MAKE) -C . -f ../Makefile $@

clean:
	$(MAKE) -C . -f ../Makefile $@
run:
	$(MAKE) -C . -f ../Makefile $@
mostlyclean:
	$(MAKE) -C . -f ../Makefile $@
testrun:
	$(MAKE) -C . -f ../Makefile $@
br:
	$(MAKE) -C . -f ../Makefile $@

release: export COMMON_CFLAGS=-O3 -Wall -Werror -DRELEASE -UDEBUG
release: all
.PHONY: all clean run mostlyclean testrun br
