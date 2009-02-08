require 'mkmf'
have_header('assert.h') or exit
find_header('gcc.h', '../fast_xs')
dir_config('fast_xs_extra')
create_makefile('fast_xs_extra')
