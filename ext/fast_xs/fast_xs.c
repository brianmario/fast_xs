#include <ruby.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "ruby_1_9_compat.h"
#include "fast_xs_type.h"
#include "gcc.h"

static ID unpack_id;
static VALUE U_fmt, C_fmt;

/* pass-through certain characters for CP-1252 */
#define p(x) (x-128)

static const int cp_1252[] = {
	8364,		/* 128 => 8364, euro sign */
	p(129),		/* 129 => 129,  pass-through */
	8218,		/* 130 => 8218, single low-9 quotation mark */
	402,		/* 131 =>  402, latin small letter f with hook */
	8222,		/* 132 => 8222, double low-9 quotation mark */
	8230,		/* 133 => 8230, horizontal ellipsis */
	8224,		/* 134 => 8224, dagger */
	8225,		/* 135 => 8225, double dagger */
	710,		/* 136 =>  710, modifier letter circumflex accent */
	8240,		/* 137 => 8240, per mille sign */
	352,		/* 138 =>  352, latin capital letter s with caron */
	8249,		/* 139 => 8249, single left-pointing angle quotation mark */
	338,		/* 140 =>  338, latin capital ligature oe */
	p(141),		/* 141 =>  141, pass-through */
	381,		/* 142 =>  381, latin capital letter z with caron */
	p(143),		/* 143 =>  143, pass-through */
	p(144),		/* 144 =>  144, pass-through */
	8216,		/* 145 => 8216, left single quotation mark */
	8217,		/* 146 => 8217, right single quotation mark */
	8220,		/* 147 => 8220, left double quotation mark */
	8221,		/* 148 => 8221, right double quotation mark */
	8226,		/* 149 => 8226, bullet */
	8211,		/* 150 => 8211, en dash */
	8212,		/* 151 => 8212, em dash */
	732,		/* 152 =>  732, small tilde */
	8482,		/* 153 => 8482, trade mark sign */
	353,		/* 154 =>  353, latin small letter s with caron */
	8250,		/* 155 => 8250, single right-pointing angle quotation mark */
	339,		/* 156 =>  339, latin small ligature oe */
	p(157),		/* 157 =>  157, pass-through */
	382,		/* 158 =>  382, latin small letter z with caron */
	376		/* 159 =>  376} latin capital letter y with diaeresis */
};

#define VALID_VALUE(n) \
	(n >= 0x20 && n <= 0xD7FF) || \
	    (n >= 0xE000 && n <= 0xFFFD) || \
	    (n >= 0x10000 && n <= 0x10FFFF)

#define CP_1252_ESCAPE(n) do { \
	if (n >= 128 && n <= 159) \
		n = cp_1252[n - 128]; \
	} while(0)

static inline size_t bytes_for(int n)
{
	if (n < 1000)
		return sizeof("&#999;") - 1;
	if (n < 10000)
		return sizeof("&#9999;") - 1;
	if (n < 100000)
		return sizeof("&#99999;") - 1;
	if (n < 1000000)
		return sizeof("&#999999;") - 1;
	/* if (n < 10000000), we won't have cases above 0x10FFFF */
	return sizeof("&#9999999;") - 1;
}

static size_t escape(char *buf, int n)
{

#define return_const_len(x) do { \
	memcpy(buf, x, sizeof(x) - 1); \
	return (sizeof(x) - 1); \
} while (0)

	/* handle ASCII first */
	if (likely(n < 128)) {
		if (likely(n >= 0x20 || n == '\t' || n == '\n' || n == '\r')) {
			if (unlikely(n == '&'))
				return_const_len("&amp;");
			if (unlikely(n == '<'))
				return_const_len("&lt;");
			if (unlikely(n == '>'))
				return_const_len("&gt;");
			buf[0] = (char)n;
			return 1;
		}

		buf[0] = '*';
		return 1;
	}

#undef return_const_len

	CP_1252_ESCAPE(n);

	if (VALID_VALUE(n)) {
		/* return snprintf(buf, sizeof("&#1114111;"), "&#%i;", n); */
		extern const char ruby_digitmap[];
		size_t rv = sizeof("&#;") - 1;
		buf += bytes_for(n);
		*--buf = ';';
		do {
			*--buf = ruby_digitmap[(int)(n % 10)];
			++rv;
		} while (n /= 10);
		*--buf = '#';
		*--buf = '&';
		return rv;
	}
	buf[0] = '*';
	return 1;
}

static VALUE unpack_utf8(VALUE self)
{
	return rb_funcall(self, unpack_id, 1, U_fmt);
}

static VALUE unpack_uchar(VALUE self)
{
	return rb_funcall(self, unpack_id, 1, C_fmt);
}

static VALUE fast_xs(VALUE self)
{
	long i;
	VALUE array;
	char *c;
	size_t s_len;
	VALUE *tmp;
	VALUE rv;

	array = rb_rescue(unpack_utf8, self, unpack_uchar, self);

	for (tmp = RARRAY_PTR(array), s_len = i = RARRAY_LEN(array);
	     --i >= 0;
	     tmp++) {
		int n = NUM2INT(*tmp);
		if (likely(n < 128)) {
			if (unlikely(n == '&'))
				s_len += (sizeof("&amp;") - 2);
			if (unlikely(n == '>' || n == '<'))
				s_len += (sizeof("&gt;") - 2);
			continue;
		}

		CP_1252_ESCAPE(n);

		if (VALID_VALUE(n))
			s_len += bytes_for(n) - 1;
	}

	rv = rb_str_new(NULL, s_len);
	c = RSTRING_PTR(rv);

	for (tmp = RARRAY_PTR(array), i = RARRAY_LEN(array); --i >= 0; tmp++)
		c += escape(c, NUM2INT(*tmp));

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

	rv = rb_str_new(NULL, new_len);
	new_str = RSTRING_PTR(rv);

#define append_const(buf, x) do { \
	buf = memcpy(buf, x, sizeof(x) - 1) + sizeof(x) - 1; \
} while (0)

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (unlikely(*s == '&'))
			append_const(new_str, "&amp;");
		else if (unlikely(*s == '<'))
			append_const(new_str, "&lt;");
		else if (unlikely(*s == '>'))
			append_const(new_str, "&gt;");
		else if (unlikely(*s == '"'))
			append_const(new_str, "&quot;");
		else
			*new_str++ = *s;
	}

#undef append_const

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

	rv = rb_str_new(NULL, new_len);
	new_str = RSTRING_PTR(rv);

	for (s = RSTRING_PTR(self), i = RSTRING_LEN(self); --i >= 0; ++s) {
		if (likely(CGI_URI_OK(*s)))
			*new_str++ = *s;
		else if (space_to_plus && *s == ' ')
			*new_str++ = '+';
		else {
			static const char cgi_digitmap[] = "0123456789ABCDEF";
			new_str[2] = cgi_digitmap[(*s % 16)];
			new_str[1] = cgi_digitmap[((*s/16) % 16)];
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

static VALUE fast_uxs_cgi(VALUE self)
{
	return _uxs_uri(self, 1);
}

void Init_fast_xs(void)
{
	assert(cp_1252[159 - 128] == 376); /* just in case I skipped a line */

	unpack_id = rb_intern("unpack");
	U_fmt = rb_str_new("U*", 2);
	C_fmt = rb_str_new("C*", 2);
	rb_global_variable(&U_fmt);
	rb_global_variable(&C_fmt);

	rb_define_method(rb_cString, "fast_xs", fast_xs, 0);
	rb_define_method(rb_cString, "fast_xs_html", fast_xs_html, 0);
	rb_define_method(rb_cString, "fast_xs_cgi", fast_xs_cgi, 0);
	rb_define_method(rb_cString, "fast_uxs_cgi", fast_uxs_cgi, 0);
	rb_define_method(rb_cString, "fast_xs_url", fast_xs_url, 0);
}
