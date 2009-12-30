require 'mkmf'
dir_config('fast_xs_extra')
have_header('assert.h') or abort 'assert.h not found'
find_header('gcc.h', File.dirname(__FILE__) + '/../fast_xs')
have_func("rb_str_set_len", "ruby.h")
have_func("rb_str_modify", "ruby.h")
create_makefile('fast_xs_extra')
