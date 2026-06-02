project(
  'supertokens',
  'cpp',
  version: '0.1.5',
  license: 'MIT',
  default_options: [
    'cpp_std=c++20',
    'buildtype=release',
    'warning_level=2',
  ],
)

py = import('python').find_installation(pure: false)
nanobind_dep = dependency('nanobind', static: true)

subdir('src')
