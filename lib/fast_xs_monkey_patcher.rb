require 'fast_xs'
require 'fast_xs_extra'

class CGI # :nodoc:
  class << self
    undef_method :escapeHTML
    undef_method :escape
    undef_method :unescape
    def escapeHTML(value); value.fast_xs_html; end
    def escape(value); value.fast_xs_cgi; end
    def unescape(value); value.fast_uxs_cgi; end
  end
end if defined?(CGI)

class ERB # :nodoc:
  module Util # :nodoc:
    undef_method :html_escape
    undef_method :h
    undef_method :url_encode
    undef_method :u

    def html_escape(value); value.to_s.fast_xs_html; end
    alias h html_escape
    module_function :h
    module_function :html_escape

    def url_encode(value); value.to_s.fast_xs_url; end
    alias u url_encode
    module_function :u
    module_function :url_encode
  end
end if defined?(ERB::Util)

module Mongrel # :nodoc:
  class HttpRequest # :nodoc:
    class << self
      undef_method :unescape
      undef_method :escape
      def unescape(s); s.fast_uxs_cgi; end
      def escape(s); s.to_s.fast_xs_cgi; end
    end
  end
end if defined?(Mongrel::HttpRequest)

module Rack # :nodoc:
  module Utils # :nodoc:
    undef_method :unescape
    def unescape(s); s.fast_uxs_cgi; end
    module_function :unescape

    undef_method :escape
    def escape(s); s.to_s.fast_xs_cgi; end
    module_function :escape
  end
end if defined?(Rack::Utils)
