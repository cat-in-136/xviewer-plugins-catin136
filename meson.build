project('xviewer-plugins-catin136', 'c', version: '0.0.7')

gnome = import('gnome')
i18n = import('i18n')
pkgconfig = import('pkgconfig')

version = meson.project_version()

proj_conf = configuration_data()
proj_conf.set_quoted('VERSION', version)
proj_conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())

# directories
prefix = get_option('prefix')
bindir = get_option('bindir')
datadir = get_option('datadir')
libdir = get_option('libdir')

src_root = meson.source_root()
po_dir = src_root / 'po'
metainfo_dir = prefix / datadir / 'metainfo'
gschemas_dir = prefix / datadir / 'glib-2.0' / 'schemas'

plugin_libdir =  prefix / 'lib' / 'xviewer' / 'plugins'
plugin_datadir = prefix / datadir / 'xviewer' / 'plugins'

exif = dependency('libexif', version: '>=0.6.16')
glib = dependency('glib-2.0', version: '>=2.32.0')
gtk = dependency('gtk+-3.0', version: '>=3.3.8')
libpeas = dependency('libpeas-1.0', version: '>=0.7.4')
libpeas_gtk = dependency('libpeas-gtk-1.0', version: '>= 1.12.0')
xviewer = dependency('xviewer', version: '>=3.2.1')

exempi = dependency('exempi-2.0', version: '>= 1.99.5', required: get_option('exempi'))
if exempi.found()
    proj_conf.set10('HAVE_EXEMPI', true)
endif

lcms = dependency('lcms2', required: get_option('lcms'))
if lcms.found()
    proj_conf.set10('HAVE_LCMS', true)
endif

proj_conf.set10('HAVE_EXIF', true)

# generate config.h
config_h_file = configure_file(
    output : 'config.h',
    configuration : proj_conf
)

config_h = declare_dependency(
    sources: config_h_file,
    include_directories: include_directories('.')
)

add_project_arguments('-DHAVE_CONFIG_H', language: 'c')
add_project_arguments('-Wno-deprecated-declarations', language: 'c')

subdir('install-scripts')
subdir('plugins')
subdir('po')
