require 'fast_xs'

if defined?(CGI)

  class CGI

    def CGI::escapeHTML(value)
      value.fast_xs_html
    end

    def CGI::escape(value)
      value.fast_xs_cgi
    end

    def CGI::unescape(value)
      value.fast_uxs_cgi
    end

  end

end

if defined?(ERB::Util)

  module ERB::Util

    def html_escape(value)
      value.to_s.fast_xs_html
    end
    alias h html_escape
    module_function :h
    module_function :html_escape

    def url_encode(value)
      value.to_s.fast_xs_url
    end
    alias u url_encode
    module_function :u
    module_function :url_encode


  end

end
