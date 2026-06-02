# 🚀 supertokens

> A high-performance tokenizer for Python — powered by a C++ extension built with [nanobind](https://github.com/wjakob/nanobind) and [Meson](https://mesonbuild.com/).

[![PyPI version](https://img.shields.io/pypi/v/supertokens.svg)](https://pypi.org/project/supertokens/)
[![Python](https://img.shields.io/pypi/pyversions/supertokens.svg)](https://pypi.org/project/supertokens/)
[![PyPI downloads](https://img.shields.io/pypi/dm/supertokens.svg)](https://pypi.org/project/supertokens/)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build](https://img.shields.io/github/actions/workflow/status/shaheen-coder/supertokens/main.yml?branch=main&label=build)](https://github.com/yourorg/supertokens/actions)

![Linux](https://img.shields.io/badge/Linux-Supported-FCC624?logo=linux&logoColor=black)

![macOS](https://img.shields.io/badge/macOS-Supported-000000?logo=apple)

![Windows](https://img.shields.io/badge/Windows-Supported-0078D6?logo=windows)
---

## ✨ Features

- ⚡ **Fast** — core tokenization in C++, zero-copy bindings via [nanobind](https://github.com/wjakob/nanobind)
- 🔤 **BPE tokenization** — Byte-Pair Encoding implementation with full Unicode support via [utfcpp](https://github.com/nemtrif/utfcpp)
- 🗺️ **Robin-map backend** — hash map operations use [tsl::robin_map](https://github.com/Tessil/robin-map) for cache-friendly performance
- 🐍 **Clean Python API** — expressive, Pythonic interface over the C++ core
- 🏗️ **Meson build system** — reproducible builds, easy subproject management

---

## 📦 Installation

### From PyPI

```bash
pip install supertokens
```

### From source

**Prerequisites:**

- Python ≥ 3.10
- C++17 compiler (`g++`, `clang++`, or MSVC 2019+)
- [Meson](https://mesonbuild.com/) ≥ 1.1 and [Ninja](https://ninja-build.org/)

```bash
git clone https://github.com/yourorg/supertokens.git
cd supertokens
pip install .
```

Or with editable/dev install:

```bash
pip install --no-build-isolation -e .
```

---

## 🔧 Building Manually (Meson)

```bash
# Configure
meson setup build --wipe

# Compile
meson compile -C build

# Run tests
meson test -C build
```

---

## 🚀 Quick Start

```python
from supertokens import BPE

# Load a pre-trained tokenizer
tokenizer = BPE.from_file("tokenizer.sha")

# Encode text
ids = tokenizer.encode("Hello, world!")
print(ids)  # [15496, 11, 995, 0]

# Decode tokens
text = tokenizer.decode(ids)
print(text)  # "Hello, world!"
```

---

## 📖 API Reference

### `supertokens.model`

Lower-level access to the underlying model data structures. See [`python/supertokens/model.py`](python/supertokens/model.py) for details.

---

## 🗂️ Project Structure

```
supertokens/
├── python/supertokens/       # Python package
│   ├── __init__.py
│   ├── BPE.py                # High-level BPE tokenizer
│   └── model.py              # Model data types
│
├── src/                      # C++ extension source
│   ├── bindings.cxx          # nanobind Python bindings
│   ├── tokenizer/
│   │   ├── bpe.cxx / .hxx    # BPE algorithm
│   │   └── datatypes.hxx     # Shared types
│   ├── utils/                # String & BPE utilities
│   └── libs/
│       ├── expected.hxx      # std::expected polyfill
│       └── utfcpp/           # UTF-8 string handling
│
├── subprojects/              # Meson subprojects (vendored)
│   ├── nanobind-2.12.0/      # Python ↔ C++ bindings
│   └── robin-map-1.4.0/      # Fast hash map
│
├── meson.build               # Top-level build definition
└── pyproject.toml            # Python package metadata
```

---

## 🔩 Dependencies

| Dependency | Version | Role |
|---|---|---|
| [nanobind](https://github.com/wjakob/nanobind) | 2.12.0 | C++ ↔ Python bindings |
| [tsl::robin-map](https://github.com/Tessil/robin-map) | 1.4.0 | High-performance hash map |
| [utfcpp](https://github.com/nemtrif/utfcpp) | bundled | UTF-8 string processing |
| [expected.hxx](https://github.com/TartanLlama/expected) | bundled | Error handling polyfill |

All C++ dependencies are vendored under `subprojects/` and managed by Meson — no manual installation required.

---

### Code style

- C++: follow the existing `.cxx`/`.hxx` style; C++17 standard
- Python: [Black](https://black.readthedocs.io/) + [Ruff](https://docs.astral.sh/ruff/)

```bash
black python/
ruff check python/
```

---

## 📋 Changelog

See [CHANGELOG.md](CHANGELOG.md) for release history.

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes
4. Open a Pull Request

For major changes, open an issue first to discuss the approach.

---

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

Third-party licenses:
- nanobind: BSD 3-Clause ([`subprojects/nanobind-2.12.0/LICENSE`](subprojects/nanobind-2.12.0/LICENSE))
- tsl::robin-map: MIT ([`subprojects/robin-map-1.4.0/LICENSE`](subprojects/robin-map-1.4.0/LICENSE))
