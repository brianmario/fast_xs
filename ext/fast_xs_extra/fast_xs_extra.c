#include <ruby.h>
#include <assert.h>
#include "ruby_1_9_compat.h"
#include "fast_xs_type.h"
#include "gcc.h"

#define APPEND_CONST(buf, x) do { \
	buf = memcpy(buf, x, sizeof(x) - 1) + sizeof(x) - 1; \
} while (0)

/* we are locale agnostic, unlike ctype.h functions */

static inline int is_ascii_digit(const int c)
{
	return (c >= '0' && c <= '9');
}

static inline int is_ascii_alpha(const int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline int is_ascii_alnum(const int c)
{
	return (is_ascii_alpha(c) || is_ascii_digit(c));
}

static long check_xs(char *s, long i)
{
	long orig = i;

	if (i < 2)
		return 0;
	++s;
	--i;
	if (*s == '#')
		while (--i >= 0 && is_ascii_digit(*++s)) ;
	else if (is_ascii_alnum(*s))
		while (--i >= 0 && is_ascii_alnum(*++s)) ;
	else
		return 0;

	if (*s != ';')
		return 0;

	return orig - i + 1;
}

static inline VALUE _xs_html(VALUE self, long once)
{
	long i;
	char *s;
	char *new_str;
	VALUE rv;
	char *orig_ptr = RSTRING_PTR(self);
	long orig_len = RSTRING_LEN(self);
	long new_len = orig_len;
	long int saved = 0;
	long n;

	for (s = orig_ptr, i = orig_len; --i >= 0; ++s) {
		if (unlikely(*s == '&'))
			new_len += (sizeof("&amp;") - 2);
		else if (unlikely(*s == '<' || *s == '>'))
			new_len += (sizeof("&gt;") - 2);
		else if (unlikely(*s == '"'))
			new_len += (sizeof("&quot;") - 2);
	}

	rv = rb_str_new(NULL, new_len);
	new_str = RSTRING_PTR(rv);

	for (s = orig_ptr, i = orig_len; --i >= 0; ++s) {
		if (likely(is_ascii_alnum(*s))) {
			*new_str++ = *s;
			continue;
		}
		switch (*s) {
		case '"':
			APPEND_CONST(new_str, "&quot;");
			break;
		case '&':
			n = once ? check_xs(s, i) : 0;
			if (n) {
				new_str = memcpy(new_str, s, n) + n;
				--n;
				s += n;
				i -= n;
				saved += sizeof("&amp;") - sizeof("&");
			} else {
				APPEND_CONST(new_str, "&amp;");
			}
			break;
		case '<':
			APPEND_CONST(new_str, "&lt;");
			break;
		case '>':
			APPEND_CONST(new_str, "&gt;");
			break;
		default:
			*new_str++ = *s;
		}
	}
	if (saved)
		rb_str_set_len(rv, new_len - saved);

	return rv;
}

/*
 * This is coding agnostic, and works on each byte, so some multibyte
 * character sets may not be fully supported (but UTF-8 should be).
 * This is meant to be 100% compatible with the
 * ERB::Util::escape_html and CGI::escapeHTML methods
 */
static VALUE fast_xs_html(VALUE self)
{
	return _xs_html(self, 0);
}

/*
 * This is coding agnostic, and works on each byte, so some multibyte
 * character sets may not be fully supported (but UTF-8 should be).
 */
static VALUE fast_xs_html_once(VALUE self)
{
	return _xs_html(self, 1);
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

	rv = rb_str_new(NULL, new_len);
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

	rv = rb_str_new(NULL, new_len);
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
	rb_define_method(rb_cString, "fast_xs_html_once", fast_xs_html_once, 0);
	rb_define_method(rb_cString, "fast_xs_cgi", fast_xs_cgi, 0);
	rb_define_method(rb_cString, "fast_uxs_cgi", fast_uxs_cgi, 0);
	rb_define_method(rb_cString, "fast_xs_url", fast_xs_url, 0);
}
