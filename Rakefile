require 'hoe'
hoe = Hoe.spec('fast_xs') do
  self.version = '0.8.0'
  self.author = 'fast_xs hackers'
  self.email = 'fast-xs-general@rubyforge.org'
  self.url = 'http://fast-xs.rubyforge.org/'
  self.history_file = 'History.rdoc'
  self.readme_file = 'README.rdoc'
  self.remote_rdoc_dir = ''
  self.spec_extras = { :extensions => Dir.glob('ext/*/extconf.rb') }
  self.extra_rdoc_files = %w(
    History.rdoc
    README.rdoc
    ext/fast_xs/fast_xs.c
    ext/fast_xs_extra/fast_xs_extra.c
    lib/fast_xs_monkey_patcher.rb
  )
end

# optional rake-compiler support in case somebody needs to cross compile
if RUBY_PLATFORM =~ /java/
  begin
    require 'rake/javaextensiontask'
    Rake::JavaExtensionTask.new('fast_xs', hoe.spec)
    Rake::JavaExtensionTask.new('fast_xs_extra', hoe.spec)
  rescue LoadError
    warn "rake-compiler not available, cross compiling disabled"
  end
else
  begin
    require 'rake/extensiontask'
    Rake::ExtensionTask.new('fast_xs', hoe.spec)
    Rake::ExtensionTask.new('fast_xs_extra', hoe.spec)
  rescue LoadError
    warn "rake-compiler not available, cross compiling disabled"
  end
end
