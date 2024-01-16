CC?=cc
COMMON_CFLAGS=-Wall -fPIC -Dno_tests
DEPFLAGS?=-MMD -MP
CCLD?=cc
LDFLAGS?=-pie
LIBS?=-lSDL2 -lfreetype -lpng16
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
TESTLAUNCHER_SRC=testlauncher.c 
SRCS=$(filter-out $(TESTLAUNCHER_SRC),$(wildcard ./*.c)) 
OBJS=$(patsubst %,$(OBJDIR)/%.o,$(shell find . -maxdepth 1 -name "*.c" | grep -v $(TESTLAUNCHER_SRC) | xargs basename -as .c))
DEPS=$(patsubst %.o,%.d,$(OBJS))
TESTLAUNCHER_EXE_NAME=testlauncher
TESTLAUNCHER_EXE=$(BINDIR)/$(EXEPREFIX)$(TESTLAUNCHER_EXE_NAME)$(EXESUFFIX)
EXE=$(BINDIR)/$(EXEPREFIX)zapalki$(EXESUFFIX)
TESTARGS=1+4-5+1948=5342-134

.PHONY: all release compile link strip clean mostlyclean update run br testrun
.NOTPARALLEL: all release br

all: CFLAGS = -ggdb -O0 -Wall
all: $(OBJDIR) $(BINDIR) compile link $(TESTLAUNCHER_EXE)

release: CFLAGS = -O3 -Wall -Werror -DRELEASE
release: clean $(OBJDIR) $(BINDIR) compile link mostlyclean strip run

br: all testrun

link: $(OBJS)
	@$(PRINTF) "CCLD 	%-20s %-20s\n" "$(EXE)" "<= $^"
	@$(CCLD) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

$(TESTLAUNCHER_EXE): $(TESTLAUNCHER_SRC)
	@$(PRINTF) "CCLD 	%-20s %-20s\n" "$(TESTLAUNCHER_EXE)" "<= $^"
	@$(CC) -o $(TESTLAUNCHER_EXE) $(TESTLAUNCHER_SRC)

compile: $(OBJS)

$(OBJDIR):
	@$(ECHO) "MKDIR	$(OBJDIR)"
	@$(MKDIR) $(OBJDIR)

$(BINDIR):
	@$(ECHO) "MKDIR	$(BINDIR)"
	@$(MKDIR) $(BINDIR)

$(OBJDIR)/%.o: ./%.c Makefile
	@$(PRINTF) "CC 	%-20s %-20s\n" "$@" "<= $<"
	@$(CC) $(DEPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: */%.c Makefile
	@$(PRINTF) "CC 	%-20s %-20s\n" "$@" "<= $<"
	@$(CC) $(DEPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -c -o $@ $<

strip:
	@$(ECHO) "STRIP	$(EXE)"
	@$(STRIP) $(STRIPFLAGS) $(EXE)

mostlyclean:
	@$(ECHO) "RM	$(OBJS) $(DEPS)"
	@$(RM) $(OBJS) $(DEPS)

clean:
	@$(ECHO) "RM	$(OBJS) $(DEPS) $(EXE) $(TESTLAUNCHER_EXE)"
	@$(RM) $(OBJS) $(DEPS) $(EXE) $(TESTLAUNCHER_EXE)

update:
	@$(ECHO) "TOUNCH	$(SRCS)"
	@$(TOUNCH) $(SRCS)

testrun: $(EXE) $(TESTLAUNCHER_EXE)
	@$(ECHO) "EXEC	$(TESTLAUNCHER_EXE)"
	@$(CD) $(BINDIR) && \
		PATH=. $(EXEC) $(TESTLAUNCHER_EXE_NAME)

run:
	@$(ECHO) "EXEC	$(EXE) $(TESTARGS)"
	@$(EXEC) $(EXE) $(TESTARGS)

-include $(DEPS)
