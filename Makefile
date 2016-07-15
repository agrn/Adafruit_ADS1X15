LD = $(CC)
BUILDDIR ?= build

CFLAGS += -w -Wall -fPIC -DPIC -fstack-protector
LDFLAGS += -Wl,-undefined -Wl,dynamic_lookup -pie

LIBS = $(patsubst %.c,$(BUILDDIR)/libs/%.o,$(wildcard *.c))

EXAMPLES=$(patsubst %/,%,$(dir $(patsubst examples/%,%,$(wildcard examples/*/*.c))))
EXAMPLES_OBJS=$(addprefix $(BUILDDIR)/examples/objs/,$(EXAMPLES:%=%.o))
EXAMPLES_EXECUTABLES=$(addprefix $(BUILDDIR)/examples/,$(EXAMPLES))

ifeq ($(MODE),release)
	CFLAGS += -Werror -O2
	LDFLAGS += -s
else
	CFLAGS += -g
	LDFLAGS += -g
endif

all: exampledir libdir $(LIBS) $(EXAMPLES_EXECUTABLES)

rebuild: clean all

exampledir: builddir
	@echo " MKDIR	$(BUILDDIR)/examples/"
	@mkdir -p $(BUILDDIR)/examples/
	@echo " MKDIR	$(BUILDDIR)/examples/objs/"
	@mkdir -p $(BUILDDIR)/examples/objs/

libdir: builddir
	@echo " MKDIR	$(BUILDDIR)/libs/"
	@mkdir -p $(BUILDDIR)/libs/

$(BUILDDIR)/examples/%: $(BUILDDIR)/examples/objs/%.o $(LIBS)
	@echo " LD	$@"
	@$(LD) $< $(LIBS) $(LDFLAGS) -o $@

$(BUILDDIR)/examples/objs/%.o: examples/*/%.c exampledir
	@echo " CC	$@"
	@$(CC) $< -I. $(CFLAGS) -c -o $@

$(BUILDDIR)/libs/%.o: %.c libdir
	@echo " CC	$@"
	@$(CC) $< $(CFLAGS) -c -o $@

clean:
	@echo " RM	$(LIBS) $(EXAMPLES_OBJS) $(EXAMPLES_EXECUTABLES)"
	@rm -rf $(LIBS) $(EXAMPLES_OBJS) $(EXAMPLES_EXECUTABLES)

clean_after: all
	@echo " RM	$(BUILDDIR)/examples/objs/"
	@rm -rf $(BUILDDIR)/examples/objs/

clean_all_after: clean_after
	@echo " RM	$(BUILDDIR)/libs/"
	@rm -rf $(BUILDDIR)/libs/

mrproper:
	@echo " RM	$(BUILDDIR)/"
	@rm -rf $(BUILDDIR)/

.PHONY: all rebuild builddir exampledir libdir clean clean_after clean_all_after mrproper
.SECONDARY: $(EXAMPLES_OBJS)
