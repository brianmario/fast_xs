require 'rack'
$orig_escape = Rack::Utils.method(:escape)
$orig_unescape = Rack::Utils.method(:unescape)
require 'test/unit'
require 'fast_xs_monkey_patcher'

# copied and translated to Test::Unit from test/spec_utils.rb in rack

class TestRackUtilModuleOverrides < Test::Unit::TestCase

  def test_overidden
    assert $orig_escape != Rack::Utils.method(:escape)
    assert $orig_unescape != Rack::Utils.method(:unescape)
  end

  def test_escape_html_predefined
    assert_equal "fo%3Co%3Ebar", Rack::Utils.escape("fo<o>bar")
    assert_equal "a+space", Rack::Utils.escape("a space")
    assert_equal "q1%212%22%27w%245%267%2Fz8%29%3F%5C",
                  Rack::Utils.escape("q1!2\"'w$5&7/z8)?\\")

    matz_name = # Matsumoto
      "\xE3\x81\xBE\xE3\x81\xA4\xE3\x82\x82\xE3\x81\xA8".unpack("a*")[0]
    matz_name.force_encoding("UTF-8") if matz_name.respond_to? :force_encoding
    assert_equal '%E3%81%BE%E3%81%A4%E3%82%82%E3%81%A8',
                 Rack::Utils.escape(matz_name)

    matz_name_sep = # Matsu moto
      "\xE3\x81\xBE\xE3\x81\xA4 \xE3\x82\x82\xE3\x81\xA8".unpack("a*")[0]
    matz_name_sep.respond_to?(:force_encoding) and
        matz_name_sep.force_encoding("UTF-8")
    assert_equal '%E3%81%BE%E3%81%A4+%E3%82%82%E3%81%A8',
                 Rack::Utils.escape(matz_name_sep)
  end

  def test_unescape
    assert_equal "fo<o>bar", Rack::Utils.unescape("fo%3Co%3Ebar")
    assert_equal "a space", Rack::Utils.unescape("a+space")
    assert_equal "a space", Rack::Utils.unescape("a%20space")
    assert_equal "q1!2\"'w$5&7/z8)?\\",
                 Rack::Utils.unescape("q1%212%22%27w%245%267%2Fz8%29%3F%5C")
  end
end
