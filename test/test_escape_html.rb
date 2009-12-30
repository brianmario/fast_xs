require 'test/unit'
require 'fast_xs_extra'

class TestEscapeHtml< Test::Unit::TestCase

  def test_escape_html_predefined
    assert_equal '&amp;', '&'.fast_xs_html_once
    assert_equal '&amp;', '&'.fast_xs_html
    assert_equal '&quot;', '"'.fast_xs_html_once
    assert_equal '&quot;', '"'.fast_xs_html
    assert_equal '&lt;', '<'.fast_xs_html_once
    assert_equal '&lt;', '<'.fast_xs_html
    assert_equal '&gt;', '>'.fast_xs_html_once
    assert_equal '&gt;', '>'.fast_xs_html
  end

  def test_escape_html_normal
    assert_equal 'hello world', 'hello world'.fast_xs_html
    assert_equal 'hello world', 'hello world'.fast_xs_html_once
    assert_equal '', ''.fast_xs_html
    assert_equal '', ''.fast_xs_html_once
  end

  def test_once
    assert_equal '&amp;&amp;', '&&amp;'.fast_xs_html_once
    assert_equal '&quot;&amp;hello', '&quot;&amp;hello'.fast_xs_html_once
    assert_equal '&quot;&amp;', '&quot;&amp;'.fast_xs_html_once
    assert_equal '&amp;', '&amp;'.fast_xs_html_once
    assert_equal '&#039;', '&#039;'.fast_xs_html_once
    assert_equal '&frac14;', '&frac14;'.fast_xs_html_once
    assert_equal '&amp;frac14', '&frac14'.fast_xs_html_once
    assert_equal '&amp;frac14&frac14;', '&frac14&frac14;'.fast_xs_html_once
  end

end
