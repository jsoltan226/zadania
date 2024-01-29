CC?=cc
CXX?=c++
COMMON_CFLAGS?=-Wall -fPIC -Dno_tests
DEPFLAGS?=-MMD -MP
CCLD?=cc
CXXLD?=c++
LDFLAGS?=-pie
LIBS?=
STRIP?=strip
STRIPFLAGS?=-g -s

CD=cd
ECHO=echo
PRINTF=printf
RM=rm -f
TOUNCH=touch -c
EXEC=exec
MKDIR=mkdir -p
RMDIR=rmdir

EXEPREFIX =
EXESUFFIX =
OBJDIR = obj
BINDIR = bin
CXXMIRROR_SRCDIR=cpp-src-mirror
TESTLAUNCHER_SRC=testlauncher.c 
SRCS=$(filter-out $(TESTLAUNCHER_SRC),$(wildcard ./*.c)) 
CXXSRCS=$(wildcard ./*.cpp)
OBJS=$(patsubst %,$(OBJDIR)/%.c.o,$(shell find . -maxdepth 1 -name "*.c" | grep -v $(TESTLAUNCHER_SRC) | xargs basename -as .c 2>/dev/null))
CXXOBJS=$(patsubst %,$(OBJDIR)/%.cpp.o,$(shell find . -maxdepth 1 -name "*.cpp" | xargs basename -as .cpp 2>/dev/null))
DEPS=$(patsubst %.o,%.d,$(OBJS))
CXXDEPS=$(patsubst %.o,%.d,$(CXXOBJS))
TESTLAUNCHER_EXE_NAME=testlauncher
TESTLAUNCHER_EXE=$(BINDIR)/$(EXEPREFIX)$(TESTLAUNCHER_EXE_NAME)$(EXESUFFIX)
EXE=$(BINDIR)/$(EXEPREFIX)zapalki$(EXESUFFIX)
TESTARGS=1+4+4+4-4-5+1948=5342-134+134+5342

.PHONY: all release compile link strip clean mostlyclean update run br testrun cpptest cpptest_clean cpptest_release
.NOTPARALLEL: all release br cpptest

all: CFLAGS = -ggdb -O0 -Wall -DDEBUG -URELEASE
all: $(OBJDIR) $(BINDIR) compile link $(TESTLAUNCHER_EXE)

release: CFLAGS = -O3 -Wall -Werror -DRELEASE -UDEBUG
release: clean $(OBJDIR) $(BINDIR) compile cpptest_release cpptest_clean link strip mostlyclean run

br: all testrun

link: $(OBJS) $(CXXOBJS)
	@$(PRINTF) "LD 	%-20s %-20s\n" "$(EXE)" "<= $^"
	@$(CXXLD) $(LDFLAGS) -o $(EXE) $(OBJS) $(CXXOBJS) $(LIBS)

$(TESTLAUNCHER_EXE): $(TESTLAUNCHER_SRC)
	@$(PRINTF) "CCLD 	%-20s %-20s\n" "$(TESTLAUNCHER_EXE)" "<= $^"
	@$(CC) -o $(TESTLAUNCHER_EXE) $(TESTLAUNCHER_SRC)

compile: $(OBJS) $(CXXOBJS)

$(OBJDIR):
	@$(ECHO) "MKDIR	$(OBJDIR)"
	@$(MKDIR) $(OBJDIR)

$(BINDIR):
	@$(ECHO) "MKDIR	$(BINDIR)"
	@$(MKDIR) $(BINDIR)

$(OBJDIR)/%.c.o: ./%.c Makefile
	@$(PRINTF) "CC 	%-20s %-20s\n" "$@" "<= $<"
	@$(CC) $(DEPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.cpp.o: ./%.cpp Makefile
	@$(PRINTF) "CXX 	%-20s %-20s\n" "$@" "<= $<"
	@$(CXX) $(DEPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -c -o $@ $<

strip:
	@$(ECHO) "STRIP	$(EXE)"
	@$(STRIP) $(STRIPFLAGS) $(EXE)

mostlyclean:
	@$(ECHO) "RM	$(OBJS) $(CXXOBJS) $(DEPS) $(CXXDEPS)"
	@$(RM) $(OBJS) $(CXXOBJS) $(DEPS) $(CXXDEPS)

clean:
	@$(ECHO) "RM	$(OBJS) $(CXXOBJS) $(DEPS) $(CXXDEPS) $(EXE) $(TESTLAUNCHER_EXE)"
	@$(RM) $(OBJS) $(CXXOBJS) $(DEPS) $(CXXDEPS) $(EXE) $(TESTLAUNCHER_EXE)

update:
	@$(ECHO) "TOUNCH	$(SRCS)"
	@$(TOUCH) $(SRCS)

testrun: $(EXE) $(TESTLAUNCHER_EXE)
	@$(ECHO) "EXEC	$(TESTLAUNCHER_EXE)"
	@$(CD) $(BINDIR) && \
		PATH=. $(EXEC) $(TESTLAUNCHER_EXE_NAME)

run:
	@$(ECHO) "EXEC	$(EXE) $(TESTARGS)"
	@$(EXEC) $(EXE) $(TESTARGS)

cpptest:
	@$(MAKE) -C $(CXXMIRROR_SRCDIR)

cpptest_release:
	@$(MAKE) -C $(CXXMIRROR_SRCDIR) release

cpptest_clean:
	@$(MAKE) -C $(CXXMIRROR_SRCDIR) mostlyclean

-include $(DEPS)
-include $(CXXDEPS)
