case RUBY_ENGINE
  when 'jruby'
    begin
      require 'fast_xs.jar'
    rescue LoadError
      require File.expand_path('../fast_xs.jar', __FILE__)
    end
    Java::fast_xs.init(JRuby.runtime)
  else
    begin
      require 'fast_xs.so'
    rescue LoadError
      require File.expand_path('../fast_xs.so', __FILE__)
    end
end
