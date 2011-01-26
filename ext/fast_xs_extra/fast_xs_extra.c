#include <ruby.h>
#include <assert.h>
#include "ruby_1_9_compat.h"
#include "fast_xs_type.h"
#include "gcc.h"

#define APPEND_CONST(buf, x) do { \
	buf = (char *)memcpy(buf, x, sizeof(x) - 1) + sizeof(x) - 1; \
} while (0)

/*
 * This is coding agnostic, and works on each byte, so some multibyte
 * character sets may not be fully supported (but UTF-8 should be).
 * This is meant to be 100% compatible with the
 * ERB::Util::escape_html and CGI::escapeHTML methods
 */
static VALUE fast_xs_html(VALUE self)
{
	long i;
	char *s;
	size_t new_len = RSTRING_LEN(self);
	char *new_str;
	VALUE rv;

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (unlikely(*s == '&'))
			new_len += (sizeof("&amp;") - 2);
		else if (unlikely(*s == '<' || *s == '>'))
			new_len += (sizeof("&gt;") - 2);
		else if (unlikely(*s == '"'))
			new_len += (sizeof("&quot;") - 2);
	}

	rv = fast_xs_buf_new(self, new_len);
	new_str = RSTRING_PTR(rv);

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (unlikely(*s == '&'))
			APPEND_CONST(new_str, "&amp;");
		else if (unlikely(*s == '<'))
			APPEND_CONST(new_str, "&lt;");
		else if (unlikely(*s == '>'))
			APPEND_CONST(new_str, "&gt;");
		else if (unlikely(*s == '"'))
			APPEND_CONST(new_str, "&quot;");
		else
			*new_str++ = *s;
	}

	return rv;
}

#define CGI_URI_OK(x) \
	((x >= 'a' && x <= 'z') || \
	 (x >= 'A' && x <= 'Z') || \
	 (x >= '0' && x <= '9') || \
	 (x == '.' || x == '-' || x == '_'))

static inline VALUE _xs_uri_encode(VALUE self, const unsigned int space_to_plus)
{
	long i;
	char *s;
	size_t new_len = RSTRING_LEN(self);
	char *new_str;
	VALUE rv;

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (likely(CGI_URI_OK(*s)) || (space_to_plus && *s == ' '))
			continue;
		new_len += 2;
	}

	rv = fast_xs_buf_new(self, new_len);
	new_str = RSTRING_PTR(rv);

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (likely(CGI_URI_OK(*s)))
			*new_str++ = *s;
		else if (space_to_plus && *s == ' ')
			*new_str++ = '+';
		else {
			static const char cgi_digitmap[] = "0123456789ABCDEF";
			unsigned char c = *s;

			new_str[2] = cgi_digitmap[(c % 16)];
			new_str[1] = cgi_digitmap[((c/16) % 16)];
			new_str[0] = '%';
			new_str += 3;
		}
	}

	return rv;
}

/*
 * Compatible with ERB::Util::url_encode / ERB::Util::u, this iterates
 * through each byte, so multibyte character sets may not supported (but
 * UTF-8 should be).
 */
static VALUE fast_xs_url(VALUE self)
{
	return _xs_uri_encode(self, 0);
}

/*
 * Compatible with CGI::escape(), this iterates through each byte, so
 * multibyte character sets may not supported (but UTF-8 should be).
 */
static VALUE fast_xs_cgi(VALUE self)
{
	return _xs_uri_encode(self, 1);
}

static VALUE _uxs_uri(VALUE self, const unsigned int plus_to_space)
{
	char *s, *new_str;
	long i;
	size_t new_len = RSTRING_LEN(self);
	VALUE rv;

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self);
	     --i >= 0;
	     ++s) {
		if (unlikely(*s == '%') &&
		    likely(is_hex(s[1])) &&
		    likely(is_hex(s[2]))) {
			new_len -= 2;
			s += 2;
			i -= 2;
		}
	}

	rv = fast_xs_buf_new(self, new_len);
	new_str = RSTRING_PTR(rv);

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self);
	     --i >= 0;
	     ++s, ++new_str) {
		if (plus_to_space && unlikely(*s == '+'))
			*new_str = ' ';
		else if (unlikely(*s == '%') &&
		         likely(is_hex(s[1])) &&
			 likely(is_hex(s[2]))) {
			*new_str = hexpair_to_int(s[1], s[2]);
			s += 2;
			i -= 2;
		} else
			*new_str = *s;
	}

	return rv;
}

/* Unescapes CGI, converting plus bytes '+' to space ' ' */
static VALUE fast_uxs_cgi(VALUE self)
{
	return _uxs_uri(self, 1);
}

void Init_fast_xs_extra(void)
{
	rb_define_method(rb_cString, "fast_xs_html", fast_xs_html, 0);
	rb_define_method(rb_cString, "fast_xs_cgi", fast_xs_cgi, 0);
	rb_define_method(rb_cString, "fast_uxs_cgi", fast_uxs_cgi, 0);
	rb_define_method(rb_cString, "fast_xs_url", fast_xs_url, 0);
}
