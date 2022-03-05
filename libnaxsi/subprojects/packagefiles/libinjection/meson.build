project('libinjection', 'c',
  license: ['BSD-3-Clause'],
  default_options : ['c_std=c99', 'buildtype=release', 'b_vscrt=from_buildtype'],
  version: '49904c42a6e68dc8f16c022c693e897e4010a06c',
  meson_version: '>=0.59.0'
)

libinjection_files = [
  'src' / 'libinjection_sqli.c',
  'src' / 'libinjection_xss.c',
  'src' / 'libinjection_html5.c',
]

libinjection_includes = [
  'src',
]

libinjection = library('injection', libinjection_files,
  include_directories: include_directories(libinjection_includes),
  implicit_include_directories: false,
  install: false,
)

libinjection_dep = declare_dependency(
  link_with: libinjection,
  include_directories: libinjection_includes,
)
