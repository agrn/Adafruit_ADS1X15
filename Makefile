LD = $(CC)
BUILDDIR ?= build

CFLAGS += -w -Wall -fPIC -DPIC -fstack-protector
LDFLAGS += -Wl,-undefined -Wl,dynamic_lookup -pie

LIBS = $(patsubst %.c,$(BUILDDIR)/libs/%.o,$(wildcard *.c))
EXAMPLES=$(addprefix $(BUILDDIR)/examples/,$(patsubst %/,%,$(dir $(patsubst examples/%,%,$(wildcard examples/*/*.c)))))

ifeq ($(MODE),release)
	CFLAGS += -Werror -O2
	LDFLAGS += -s
else
	CFLAGS += -g
	LDFLAGS += -g
endif

all: exampledir libdir $(LIBS) $(EXAMPLES)

rebuild: clean all

exampledir: builddir
	@echo " MKDIR	$(BUILDDIR)/examples/"
	@mkdir -p $(BUILDDIR)/examples/

libdir: builddir
	@echo " MKDIR	$(BUILDDIR)/libs/"
	@mkdir -p $(BUILDDIR)/libs/

$(BUILDDIR)/examples/%: examples/*/%.c exampledir $(LIBS)
	@echo " CC	$@"
	@$(CC) $< $(LIBS) -I. $(CFLAGS) $(LDFLAGS) -o $@

$(BUILDDIR)/libs/%.o: %.c libdir
	@echo " CC	$@"
	@$(CC) $< $(CFLAGS) -c -o $@

clean:
	@echo " RM	$(LIBS) $(EXAMPLES)"
	@rm -rf $(LIBS) $(EXAMPLES)

mrproper:
	@echo " RM	$(BUILDDIR)/"
	@rm -rf $(BUILDDIR)/

.PHONY: all rebuild builddir exampledir libdir clean mrproper
