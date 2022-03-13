project('libnaxsi', 'c',
  version: '1.4.0',
  license: 'GPL3',
  meson_version: '>=0.59.0',
  default_options: ['c_std=c99', 'buildtype=release', 'b_vscrt=from_buildtype']
)
cc = meson.get_compiler('c')

libnaxsi_c_args = []
libnaxsi_deps = []
libnaxsi_src = [
  'src' / 'naxsi.c',
  'src' / 'naxsi_inet.c',
  'src' / 'naxsi_list.c',
  'src' / 'naxsi_mem.c',
  'src' / 'naxsi_regex.c',
  'src' / 'naxsi_rule.c',
  'src' / 'naxsi_str.c',
  'src' / 'naxsi_utils.c',
]
libnaxsi_includes = [
  'include',
  'src'
]
libnaxsi_includes_installdir = get_option('includedir') / 'naxsi'

pcre_sys = get_option('use_system_pcre')
pcre2_dep = dependency('libpcre2-posix', required: false)
pcre_dep = dependency('libpcre', required: false)
if pcre_sys and pcre2_dep.found()
  libnaxsi_deps += dependency('libpcre2-8', required: true)
  libnaxsi_deps += pcre2_dep
  libnaxsi_c_args += '-DUSE_PCRE2'
elif pcre_sys and pcre_dep.found()
  libnaxsi_deps += pcre_dep
  libnaxsi_c_args += '-DUSE_PCRE1'
else
  libpcre2_proj = subproject('pcre2', default_options: ['default_library=static'])
  libnaxsi_deps += libpcre2_proj.get_variable('libpcre2_posix')
  libnaxsi_deps += libpcre2_proj.get_variable('libpcre2_8')
  libnaxsi_c_args += '-DUSE_PCRE2'
endif

libinjection_proj = subproject('libinjection', default_options: ['default_library=static'], required: true)
libnaxsi_deps += libinjection_proj.get_variable('libinjection_dep')

if get_option('default_library') == 'shared'
  if cc.has_argument('-fvisibility=hidden')
    add_project_arguments('-fvisibility=hidden', language: 'c')
  endif
endif

libnaxsi = library('naxsi', libnaxsi_src,
  c_args : libnaxsi_c_args,
  dependencies: libnaxsi_deps,
  implicit_include_directories: false,
  install: true,
  include_directories: include_directories(libnaxsi_includes)
)

libnaxsi_dep = declare_dependency(
  link_with: libnaxsi,
  include_directories: include_directories(['include']),
)

if get_option('enable_tests') and not meson.is_subproject()
  tests_src_c = [
    'inet',
    'list',
    'mem',
    'regex',
    'rule',
    'str',
    'utils',
  ]
  tests_includes = [
    'include',
    'src',
    'test',
  ]
  test_dependencies = [] + libnaxsi_deps
  if get_option('b_sanitize').contains('address')
    test_dependencies += cc.find_library('asan')
  endif
  if get_option('b_sanitize').contains('undefined')
    test_dependencies += cc.find_library('ubsan')
  endif
  foreach test : tests_src_c
    exe = executable('test_@0@'.format(test), 'test' / 'test_@0@.c'.format(test),
      include_directories: include_directories(tests_includes),
      sources: libnaxsi_src,
      dependencies: test_dependencies,
      c_args : libnaxsi_c_args,
      install: false,
      install_rpath: '',
      implicit_include_directories: false
    )
    test(test, exe)
  endforeach
endif
