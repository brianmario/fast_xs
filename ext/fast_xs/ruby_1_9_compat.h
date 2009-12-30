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

#ifndef RUBINIUS
#  define rb_str_update(x) do {} while (0)
#  define rb_str_flush(x) do {} while (0)
#endif /* !RUBINIUS */

#ifndef HAVE_RB_STR_SET_LEN
#  ifdef RUBINIUS
#    define rb_str_set_len(str,len) rb_str_resize(str,len)
#  else /* 1.8.6 optimized version */
/* this is taken from Ruby 1.8.7, 1.8.6 may not have it */
static void rb_18_str_set_len(VALUE str, long len)
{
	RSTRING(str)->len = len;
	RSTRING(str)->ptr[len] = '\0';
	rb_str_flush(str);
}
#    define rb_str_set_len(str,len) rb_18_str_set_len(str,len)
#  endif /* ! RUBINIUS */
#endif /* !defined(HAVE_RB_STR_SET_LEN) */
