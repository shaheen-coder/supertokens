cpp_args = [
  '-DUTF_CPP_CPLUSPLUS=202002L',
  '-O3',
  '-DNOMINMAX',
  '-march=native',
  '-mtune=native',
  '-DNDEBUG',
  '-ffast-math',
  '-funroll-loops',
]


tokenizer_lib = static_library(
  'tokenizer',
  files(
    'tokenizer/bpe.cxx',
    'utils/bpe_utils.cxx',
    'utils/utils.cxx',
  ),
  include_directories: include_directories('libs'),
  cpp_args: cpp_args,
)

py.extension_module(
  'shatokens_api',
  sources: ['bindings.cxx'],
  link_with: tokenizer_lib,
  dependencies: [nanobind_dep],
  include_directories: include_directories('libs', 'tokenizer', 'utils'),
  install: true,
  subdir: 'supertokens',      
)

py.install_sources(
  [
    '../python/supertokens/__init__.py',
    '../python/supertokens/model.py',
    '../python/supertokens/BPE.py',
  ],
  subdir: 'supertokens',
)
