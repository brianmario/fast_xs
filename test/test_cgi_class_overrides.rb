# -*- encoding: utf-8 -*-
require 'test/unit'
require 'cgi'
require 'fast_xs_monkey_patcher'

class TestCgiClassOverrides < Test::Unit::TestCase

  def test_escape_html_predefined
    assert_equal '&amp;', CGI::escapeHTML('&')
    assert_equal '&quot;', CGI::escapeHTML('"')
    assert_equal '&lt;', CGI::escapeHTML('<')
    assert_equal '&gt;', CGI::escapeHTML('>')
  end

  def test_escape_html_normal
    assert_equal 'hello world', CGI::escapeHTML('hello world')
    assert_equal '', CGI::escapeHTML('')
  end

  def test_escape_html_ignore
    assert_equal "\x00", CGI::escapeHTML("\x00")
    assert_equal "\x0C", CGI::escapeHTML("\x0C")
    assert_equal "\xEF\xBF\xBF", CGI::escapeHTML("\xEF\xBF\xBF")
  end

  def test_escape_cgi
    assert_equal 'hello%3Dworld', CGI::escape('hello=world')
    assert_equal '+', CGI::escape(' ')
    assert_equal '%2B', CGI::escape('+')
    assert_equal '%2C', CGI::escape(',')
    assert_equal 'hello-world', CGI::escape('hello-world')
    assert_equal 'H3LL0+W0RLD', CGI::escape('H3LL0 W0RLD')
  end

  def test_escape_cgi_high
    assert_equal '%C3%A8', CGI::escape('è')
  end

  def test_unescape_cgi_high
    assert_equal 'è', CGI::unescape('%C3%A8')
  end

  def test_unescape_cgi
    assert_equal 'hello=world', CGI::unescape('hello%3Dworld')
    assert_equal ' ', CGI::unescape('+')
    assert_equal '+', CGI::unescape('%2B')
    assert_equal ',', CGI::unescape('%2C')
    assert_equal '%', CGI::unescape('%')
    assert_equal 'hello-world', CGI::unescape('hello-world')
    assert_equal 'H3LL0 W0RLD', CGI::unescape('H3LL0+W0RLD')
  end

  def test_large_strings
    if ENV['LARGE_STRING_TEST']
      assert CGI::escape('&' * (8192 * 1024))
      assert CGI::escapeHTML('&' * (8192 * 1024))
    end
  end

  def test_default_encoding_preserved
    amp = "&"
    assert_equal Encoding::UTF_8, amp.encoding
    res = CGI.escapeHTML(amp)
    assert_equal "&amp;", res
    assert_equal Encoding::UTF_8, res.encoding
  end if defined?(Encoding)

  def test_forced_encoding_preserved
    amp = "&"
    assert_nothing_raised {
      amp.force_encoding Encoding::US_ASCII
    }
    res = CGI.escapeHTML(amp)
    assert_equal "&amp;", res
    assert_equal Encoding::US_ASCII, res.encoding
  end if defined?(Encoding)
end
