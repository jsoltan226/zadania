CC?=cc
COMMON_CFLAGS=-Wall -fPIC
DEPFLAGS?=-MMD -MP
CCLD?=cc
LDFLAGS?=-pie
LIBS?=-lSDL2 -lfreetype -lpng16
STRIP?=strip
STRIPFLAGS?=-g -s

ECHO=echo
PRINTF=printf
RM=rm -f
TOUCH=touch -c
EXEC=exec
MKDIR=mkdir -p
RMDIR=rmdir

EXEPREFIX =
EXESUFFIX =
OBJDIR = obj
BINDIR = bin
SRCS=$(wildcard */*.c) $(wildcard ./*.c)
OBJS=$(patsubst %,$(OBJDIR)/%.o,$(shell find . -name "*.c" | xargs basename -as .c))
DEPS=$(patsubst %.o,%.d,$(OBJS))
EXE=$(BINDIR)/$(EXEPREFIX)main$(EXESUFFIX)

.PHONY: all release compile link strip clean mostlyclean update run br
.NOTPARALLEL: all release br

all: CFLAGS = -ggdb -O0 -Wall
all: $(OBJDIR) $(BINDIR) compile link

release: CFLAGS = -O3 -Wall -Werror
release: clean $(OBJDIR) $(BINDIR) compile link mostlyclean strip

br: all run

link: $(OBJS)
	@$(PRINTF) "CCLD 	%-20s %-20s\n" "$(EXE)" "<= $^"
	@$(CCLD) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

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
	@$(ECHO) "RM	$(OBJS) $(DEPS) $(EXE)"
	@$(RM) $(OBJS) $(DEPS) $(EXE)

update:
	@$(ECHO) "TOUCH	$(SRCS)"
	@$(TOUCH) $(SRCS)

run: $(EXE)
	@$(ECHO) "EXEC	$(EXE)"
	@$(EXEC) $(EXE)

-include $(DEPS)
