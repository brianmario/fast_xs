all: test
ext := fast_xs fast_xs_extra
RUBY = ruby
DLEXT := $(shell $(RUBY) -rrbconfig -e 'puts Config::CONFIG["DLEXT"]')

libs := $(addsuffix .$(DLEXT),$(addprefix lib/,$(ext)))
build: $(libs)

makefiles = $(addsuffix /Makefile,$(addprefix ext/,$(ext)))

%/Makefile: %/extconf.rb
	cd $(@D) && $(RUBY) extconf.rb

deps := ext/fast_xs/Makefile $(wildcard ext/fast_xs/*.[ch])
extra_deps := ext/fast_xs_extra/Makefile $(wildcard ext/fast_xs_extra/*.[ch])

ext/fast_xs/fast_xs.$(DLEXT): $(deps)
	$(MAKE) -C $(@D)
ext/fast_xs_extra/fast_xs_extra.$(DLEXT): $(extra_deps)
	$(MAKE) -C $(@D)

lib/fast_xs.$(DLEXT): ext/fast_xs/fast_xs.$(DLEXT)
lib/fast_xs_extra.$(DLEXT): ext/fast_xs_extra/fast_xs_extra.$(DLEXT)
lib/%.$(DLEXT):
	$(MAKE) -C $(<D)
	install -m 644 $(<D)/$(@F) $@
%/clean:
	! test -f ext/$(@D)/Makefile || $(MAKE) -C ext/$(@D) clean
	$(RM) ext/$(@D)/Makefile
clean: $(addsuffix /clean,$(ext))
	$(RM) $(libs)

T := $(wildcard test/test_*.rb)
test:
	$(MAKE) $(T)
$(T): $(libs)
	$(RUBY) -w -Ilib $@

.PHONY: test doc build all $(T)
