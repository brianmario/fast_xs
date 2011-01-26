require 'erb'
require 'test/unit'
require 'fast_xs_monkey_patcher'

class TestErbUtilModuleOverrides < Test::Unit::TestCase

  include ERB::Util

  def test_escape_html_predefined
    assert_equal '&amp;', html_escape('&')
    assert_equal '&quot;', html_escape('"')
    assert_equal '&lt;', html_escape('<')
    assert_equal '&gt;', html_escape('>')
  end

  def test_escape_html_normal
    assert_equal 'hello world', html_escape('hello world')
    assert_equal '', html_escape('')
  end

  def test_escape_html_ignore
    assert_equal "\x00", html_escape("\x00")
    assert_equal "\x0C", html_escape("\x0C")
    assert_equal "\xEF\xBF\xBF", html_escape("\xEF\xBF\xBF")
  end

  def test_escape_url
    assert_equal 'hello%3Dworld', url_encode('hello=world')
    assert_equal '%20', url_encode(' ')
    assert_equal '%2B', url_encode('+')
    assert_equal '%2C', url_encode(',')
    assert_equal 'hello-world', url_encode('hello-world')
    assert_equal 'H3LL0%20W0RLD', url_encode('H3LL0 W0RLD')
  end

  def test_preserve_encoding
    foo = "foo"
    foo.force_encoding Encoding::US_ASCII
    assert_equal Encoding::US_ASCII, url_encode(foo).encoding
    assert_equal Encoding::US_ASCII, html_escape(foo).encoding
    assert_equal Encoding::US_ASCII, u(foo).encoding
    assert_equal Encoding::US_ASCII, h(foo).encoding

    foo.force_encoding Encoding::BINARY
    assert_equal Encoding::BINARY, url_encode(foo).encoding
    assert_equal Encoding::BINARY, html_escape(foo).encoding
    assert_equal Encoding::BINARY, u(foo).encoding
    assert_equal Encoding::BINARY, h(foo).encoding
  end if defined?(Encoding)

  def test_large_strings
    if ENV['LARGE_STRING_TEST']
      assert u('&' * (8192 * 1024))
      assert h('&' * (8192 * 1024))
    end
  end

end


