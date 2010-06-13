= fast_xs, excessively fast escaping

* http://rubyforge.org/projects/fast-xs
* http://fast-xs.rubyforge.org/
* mailto:fast-xs-general@rubyforge.org
* git://rubyforge.org/fast-xs.git
* http://fast-xs.rubyforge.org/git?p=fast-xs.git

== DESCRIPTION:

fast_xs provides C extensions for escaping text.

The original String#fast_xs method is based on the xchar code by Sam Ruby:

* http://intertwingly.net/stories/2005/09/28/xchar.rb
* http://intertwingly.net/blog/2005/09/28/XML-Cleansing

_why also packages an older version with Hpricot (patches submitted).
The version here should be compatible with the latest version of Hpricot
code.

Ruby on Rails will automatically use String#fast_xs from either Hpricot
or this gem version with the bundled Builder package.

String#fast_xs is an almost exact translation of Sam Ruby's original
implementation (String#to_xs), but it does escape "&quot;" (which is an
optional, but all parsers are able ot handle it.  XML::Builder as
packaged in Rails 2.0 will be automatically use String#fast_xs instead
of String#to_xs available.

=== EXTRA:

'fast_xs_extra' is an experimental extension that can be used with
'fast_xs_monkey_patcher' to monkey patch methods in common libraries:

  CGI, ERB::Util, Rack::Utils, Mongrel

However, 'fast_xs_extra' does not provide a significant performance
boost like 'fast_xs' does under Ruby 1.8.

== COMPATIBILITY:

This package aims to be compatible with Ruby 1.8 and 1.9.  It is however
not as helpful in Ruby 1.9 which already provides encoding-aware string
functions.  Rubinius compatibility may happen in the future if deemed
necessary, but this extension seems to be against the spirit of the
Rubinius project.

== LICENSE:

(The MIT License)

Copyright (c) all contributors (see logs in git)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
