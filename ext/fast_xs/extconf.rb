require 'mkmf'
dir_config('fast_xs')
have_header('assert.h') or abort 'assert.h not found'
have_func("rb_str_set_len", "ruby.h")
have_func("rb_str_modify", "ruby.h")
create_makefile('fast_xs')
