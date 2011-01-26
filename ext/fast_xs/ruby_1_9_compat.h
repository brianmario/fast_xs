/* Ruby 1.8.6+ macros (for compatibility with Ruby 1.9) */
#ifndef RSTRING_PTR
#  define RSTRING_PTR(s) (RSTRING(s)->ptr)
#endif
#ifndef RSTRING_LEN
#  define RSTRING_LEN(s) (RSTRING(s)->len)
#endif
#ifndef RARRAY_PTR
#  define RARRAY_PTR(s) (RARRAY(s)->ptr)
#endif
#ifndef RARRAY_LEN
#  define RARRAY_LEN(s) (RARRAY(s)->len)
#endif

#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
static VALUE fast_xs_buf_new(VALUE orig, long len)
{
	rb_encoding *enc = rb_enc_get(orig);
	VALUE str = rb_str_new(NULL, len);

	return rb_enc_associate(str, enc);
}
#else /* ! HAVE_RUBY_ENCODING_H */
static VALUE fast_xs_buf_new(VALUE orig, long len)
{
	return rb_str_new(NULL, len);
}
#endif /* ! HAVE_RUBY_ENCODING_H */
