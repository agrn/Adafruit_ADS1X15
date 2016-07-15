LD = $(CC)
BUILDDIR ?= build

CFLAGS += -w -Wall -fPIC -DPIC -fstack-protector
LDFLAGS += -Wl,-undefined -Wl,dynamic_lookup -pie

LIBS = $(patsubst %.c,$(BUILDDIR)/libs/%.o,$(wildcard *.c))

EXAMPLES = $(patsubst %/,%,$(dir $(patsubst examples/%,%,$(wildcard examples/*/*.c))))
EXAMPLES_OBJS = $(addprefix $(BUILDDIR)/examples/objs/,$(EXAMPLES:%=%.o))
EXAMPLES_EXECUTABLES = $(addprefix $(BUILDDIR)/examples/,$(EXAMPLES))

ifeq ($(MODE),release)
	CFLAGS += -Werror -O2
	LDFLAGS += -Werror -s
else
	CFLAGS += -g
	LDFLAGS += -g
endif

all: examplesdir libsdir $(LIBS) $(EXAMPLES_EXECUTABLES)

libs: libsdir $(LIBS)

examples: all
	@echo " RM	$(BUILDDIR)/libs/"
	@rm -rf $(BUILDDIR)/libs/

examplesdir:
	@echo " MKDIR	$(BUILDDIR)/examples/"
	@mkdir -p $(BUILDDIR)/examples/
	@echo " MKDIR	$(BUILDDIR)/examples/objs/"
	@mkdir -p $(BUILDDIR)/examples/objs/

libsdir:
	@echo " MKDIR	$(BUILDDIR)/libs/"
	@mkdir -p $(BUILDDIR)/libs/

$(BUILDDIR)/examples/%: $(BUILDDIR)/examples/objs/%.o $(LIBS)
	@echo " LD	$@"
	@$(LD) $< $(LIBS) $(LDFLAGS) -o $@

$(BUILDDIR)/examples/objs/%.o: examples/*/%.c examplesdir
	@echo " CC	$@"
	@$(CC) $< -I. $(CFLAGS) -c -o $@

$(BUILDDIR)/libs/%.o: %.c libsdir
	@echo " CC	$@"
	@$(CC) $< $(CFLAGS) -c -o $@

clean:
	@echo " RM	$(LIBS) $(EXAMPLES_OBJS) $(EXAMPLES_EXECUTABLES)"
	@rm -rf $(LIBS) $(EXAMPLES_OBJS) $(EXAMPLES_EXECUTABLES)

clean_after: all
	@echo " RM	$(BUILDDIR)/examples/objs/"
	@rm -rf $(BUILDDIR)/examples/objs/

clean_all_after: examples clean_after

mrproper:
	@echo " RM	$(BUILDDIR)/"
	@rm -rf $(BUILDDIR)/

.PHONY: all libs examples examplesdir libsdir clean clean_after clean_all_after mrproper
.SECONDARY: $(EXAMPLES_OBJS)
