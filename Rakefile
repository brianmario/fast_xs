require 'hoe'
hoe = Hoe.spec('fast_xs') do
  self.version = '0.8.0'
  self.author = 'fast_xs hackers'
  self.email = 'fast-xs-general@rubyforge.org'
  self.url = 'http://fast-xs.rubyforge.org/'
  self.history_file = 'History.rdoc'
  self.readme_file = 'README.rdoc'
  self.remote_rdoc_dir = ''
  self.rubyforge_name = 'fast-xs'
  self.spec_extras = { :extensions => Dir.glob('ext/*/extconf.rb') }
end

# optional rake-compiler support in case somebody needs to cross compile
begin
  require 'rake/extensiontask'
  Rake::ExtensionTask.new('fast_xs', hoe.spec)
  Rake::ExtensionTask.new('fast_xs_extra', hoe.spec)
rescue LoadError
  warn "rake-compiler not available, cross compiling disabled"
end
