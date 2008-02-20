require 'rake'
require 'rake/clean'
require 'rake/gempackagetask'
require 'rake/rdoctask'
require 'rake/testtask'
require 'hoe' if ENV['VERSION']
require 'fileutils'
include FileUtils

name = "fast_xs"
begin
  rev = `git rev-list -1 HEAD --pretty=format:%ct`
  rev = Time.at(rev.split("\n")[1].to_i).strftime('%Y%m%d.%H%M%S')
rescue
end
version ||= ENV['VERSION'] || '0.6' + (rev && rev.length > 0 ? ".#{rev}" : '')
pkg = "#{name}-#{version}"
bin = "*.{so,o}"
archlib = "lib/#{::Config::CONFIG['arch']}"
CLEAN.include ["ext/fast_xs/#{bin}", "lib/**/#{bin}",
               'ext/fast_xs/Makefile', '**/.*.sw?', '*.gem', '.config',
               'pkg']
rdoc_opts = ['--quiet', '--title', 'fast_xs notes', '--main', 'README',
             '--inline-source']
pkg_files = %w(CHANGELOG COPYING README Rakefile) +
            Dir.glob("{test,lib}/**/*.rb") +
            Dir.glob("ext/**/*.{c,rb,h}")


spec = Gem::Specification.new do |s|
  s.name = name
  s.version = version
  s.platform = Gem::Platform::RUBY
  s.has_rdoc = true
  s.rdoc_options += rdoc_opts
  s.extra_rdoc_files = ["README", "CHANGELOG", "COPYING"]
  s.summary = "excessively fast escaping"
  s.description = s.summary
  s.author = "Eric Wong"
  s.email = 'normalperson@yhbt.net'
  s.homepage = 'http://bogonips.org/fast_xs/'
  s.files = pkg_files
  s.require_paths = [archlib, "lib"]
  s.extensions = FileList["ext/**/extconf.rb"].to_a
end

if ENV['VERSION']
  # FIXME - look for hoe replacement...
  prev = `git for-each-ref --format='%(refname)' refs/tags | tail -n1`.chomp
  sh("git shortlog #{prev}..HEAD > History.txt");
  sh('git ls-files > Manifest.txt')
  File.exists?('README.txt') or File.link('README', 'README.txt')
  hoe = Hoe.new(name, version) do |p|
    p.author = spec.author
    p.description = spec.description
    p.email = spec.email
    p.summary = spec.summary
    p.url = spec.homepage
    p.rubyforge_name = 'fast-xs'
  end
end

desc "Does a full compile, test run"
task :default => [:compile, :test]

desc "Run all the tests"
Rake::TestTask.new do |t|
  t.libs << "test" << archlib
  t.test_files = FileList['test/test_*.rb']
  t.verbose = true
end

Rake::RDocTask.new do |rdoc|
  rdoc.rdoc_dir = 'doc/rdoc'
  rdoc.options += rdoc_opts
  rdoc.main = "README"
  rdoc.rdoc_files.add ['README', 'CHANGELOG', 'COPYING' ]
end

Rake::GemPackageTask.new(spec) do |p|
  p.need_tar = true
  p.gem_spec = spec
end

['fast_xs'].each do |extension|
  ext = "ext/#{extension}"
  ext_so = "#{ext}/#{extension}.#{Config::CONFIG['DLEXT']}"
  ext_files = FileList[
    "#{ext}/*.c",
    "#{ext}/*.h",
    "#{ext}/*.rl",
    "#{ext}/extconf.rb",
    "#{ext}/Makefile",
    "lib"
  ]

  desc "Builds just the #{extension} extension"
  task extension.to_sym => ["#{ext}/Makefile", ext_so ]

  file "#{ext}/Makefile" => ["#{ext}/extconf.rb"] do
    Dir.chdir(ext) do ruby "extconf.rb" end
  end

  file ext_so => ext_files do
    Dir.chdir(ext) do sh('make') end
    mkdir_p archlib
    chmod 0644, ext_so
    cp ext_so, archlib
  end
end

task "lib" do
  directory "lib"
end

desc "Compiles the Ruby extension"
task :compile => [:fast_xs] do
  if Dir.glob(File.join(archlib,"fast_xs.*")).empty?
    STDERR.puts 'Gem failed to build'
    exit(1)
  end
end

task :install do
  sh %{rake package}
  sh %{sudo gem install pkg/#{name}-#{version}}
end

task :uninstall => [:clean] do
  sh %{sudo gem uninstall #{name}}
end
