require 'hoe'
begin
  require 'rake/extensiontask'
  Rake::ExtensionTask.new('fast_xs')
  Rake::ExtensionTask.new('fast_xs_extra')
rescue LoadError
  warn "rake-compiler not available, cross compiling disabled"
end

Hoe.spec('fast_xs') do
  self.version = '0.7.2'
  self.author = 'Eric Wong'
  self.email = 'fast-xs-general@rubyforge.org'
  self.url = 'http://fast-xs.rubyforge.org/'
  self.remote_rdoc_dir = ''
  self.rubyforge_name = 'fast-xs'
  self.spec_extras = { :extensions => Dir.glob('ext/*/extconf.rb') }
end
