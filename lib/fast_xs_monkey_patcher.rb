require 'fast_xs'

class CGI
  def CGI::escapeHTML(value); value.fast_xs_html; end
  def CGI::escape(value); value.fast_xs_cgi; end
  def CGI::unescape(value); value.fast_uxs_cgi; end
end if defined?(CGI)

module ERB::Util
  def html_escape(value); value.to_s.fast_xs_html; end
  alias h html_escape
  module_function :h
  module_function :html_escape

  def url_encode(value); value.to_s.fast_xs_url; end
  alias u url_encode
  module_function :u
  module_function :url_encode
end if defined?(ERB::Util)

class Mongrel::HttpRequest
  def self.unescape(s); s.fast_uxs_cgi; end
  def self.escape(s); s.to_s.fast_xs_cgi; end
end if defined?(Mongrel::HttpRequest)

module Rack::Utils
  def unescape(s); s.fast_uxs_cgi; end
  module_function :unescape

  def escape(s); s.to_s.fast_xs_cgi; end
  module_function :escape
end if defined?(Rack::Utils)
