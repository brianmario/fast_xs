require 'fast_xs'

if defined?(CGI)

  class CGI

    def CGI::escapeHTML(value)
      value.to_s.fast_xs_html
    end

    def CGI::escape(value)
      value.to_s.fast_xs_cgi
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

  end

end
