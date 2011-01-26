require 'fast_xs'
require 'test/unit'

# Based on Sam's original xchar.rb tests:

class TestXmlEscaping < Test::Unit::TestCase

  def test_ascii
    assert_equal 'abc', 'abc'.fast_xs
  end

  def test_predefined
    assert_equal '&amp;', '&'.fast_xs              # ampersand
    assert_equal '&lt;',  '<'.fast_xs              # left angle bracket
    assert_equal '&gt;',  '>'.fast_xs              # right angle bracket
    assert_equal '&quot;', '"'.fast_xs             # double quote
  end

  def test_invalid
    assert_equal '*', "\x00".fast_xs               # null
    assert_equal '*', "\x0C".fast_xs               # form feed
    assert_equal '*', "\xEF\xBF\xBF".fast_xs       # U+FFFF
  end

  def test_iso_8859_1
    assert_equal '&#231;', "\xE7".fast_xs          # small c cedilla
    assert_equal '&#169;', "\xA9".fast_xs          # copyright symbol
  end

  def test_win_1252
    assert_equal '&#8217;', "\x92".fast_xs         # smart quote
    assert_equal '&#8364;', "\x80".fast_xs         # euro
  end

  def test_utf8
    assert_equal '&#8217;', "\xE2\x80\x99".fast_xs # right single quote
    assert_equal '&#169;',  "\xC2\xA9".fast_xs     # copy
  end

  def test_iso_8859_1_encoding_preserved
    copyright = "\xA9"
    copyright.force_encoding Encoding::ISO_8859_1
    assert_equal '&#169;', copyright.fast_xs
    assert copyright.fast_xs.valid_encoding?
    assert_equal Encoding::ISO_8859_1, copyright.fast_xs.encoding
  end if defined?(Encoding)

  def test_win_1252_encoding_preserved
    euro = "\x80"
    euro.force_encoding Encoding::CP1252
    assert_equal '&#8364;', euro.fast_xs
    assert euro.fast_xs.valid_encoding?
    assert_equal Encoding::CP1252, euro.fast_xs.encoding
  end if defined?(Encoding)

  def test_utf8_encoding_preserved
    copy = "\xC2\xA9"
    copy.force_encoding Encoding::UTF_8
    assert_equal '&#169;',  copy.fast_xs
    assert_equal Encoding::UTF_8,  copy.fast_xs.encoding
    assert copy.fast_xs.valid_encoding?
  end if defined?(Encoding)

  def test_large_document
    if ENV['LARGE_STRING_TEST']
      assert(('&' * (8192 * 1024)).fast_xs)
      assert(('a' * (8192 * 1024)).fast_xs)
    end
  end

end
