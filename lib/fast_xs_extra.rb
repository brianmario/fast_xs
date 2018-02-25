case RUBY_ENGINE
  when 'jruby'
    begin
      require 'fast_xs_extra.jar'
    rescue LoadError
      require File.expand_path('../fast_xs_extra.jar', __FILE__)
    end
    Java::fast_xs_extra.init(JRuby.runtime)
  else
    begin
      require 'fast_xs_extra.so'
    rescue LoadError
      require File.expand_path('../fast_xs_extra.so', __FILE__)
    end
end
