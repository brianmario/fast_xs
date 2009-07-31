all: test
ext := fast_xs fast_xs_extra
ruby = ruby
DLEXT := $(shell $(ruby) -rrbconfig -e 'puts Config::CONFIG["DLEXT"]')

libs := $(addsuffix .$(DLEXT),$(addprefix lib/,$(ext)))
build: $(libs)

%/Makefile: %/extconf.rb
	cd $(@D) && $(ruby) extconf.rb
lib/fast_xs.$(DLEXT): ext/fast_xs/Makefile
lib/fast_xs_extra.$(DLEXT): ext/fast_xs_extra/Makefile
%.$(DLEXT):
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
	$(ruby) -w -Ilib $@

.PHONY: test doc build all $(T)
