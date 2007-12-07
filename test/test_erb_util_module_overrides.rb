require 'erb'
require 'test/unit'
load 'fast_xs_monkey_patcher.rb'

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

end


