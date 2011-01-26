begin
  require 'mongrel'
  $orig_escape = Mongrel::HttpRequest.method(:escape)
  $orig_unescape = Mongrel::HttpRequest.method(:unescape)
rescue SyntaxError,LoadError => e
  warn "skipping Mongrel test, Mongrel not available or broken under 1.9.2"
end

require 'test/unit'
require 'fast_xs_monkey_patcher'

class TestMongrelHttpOverrides < Test::Unit::TestCase
  def test_escape
    assert_equal "%2B", Mongrel::HttpRequest.escape("+")
  end

  def test_unescape
    assert_equal "+", Mongrel::HttpRequest.unescape("%2B")
  end
end if defined?(Mongrel::HttpRequest)
