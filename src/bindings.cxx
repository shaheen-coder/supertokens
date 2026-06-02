// bindings.cpp

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/unordered_map.h>

#include "tokenizer/bpe.hxx"

namespace nb = nanobind;

NB_MODULE(shatokens_api, m) {
    m.doc() = "Shatokens BPE tokenizer API";

    nb::class_<BPE>(m, "BPE")
        // constructor
        .def(nb::init<int>(),
             nb::arg("vocab_size"))

        // methods
        .def("train_fromfile",
             &BPE::train_fromfile,
             nb::arg("filepath"),
             nb::arg("buffer_size"))

        .def("encode",
             &BPE::encode,
             nb::arg("text"))

        .def("decode",
             &BPE::decode,
             nb::arg("ids"))

        // public members
        .def_rw("vocab_size",
                &BPE::vocab_size)

        .def_rw("vocab",
                &BPE::vocab)

        .def_rw("merge_rank",
                &BPE::merge_rank)

        .def_rw("token_to_id",
                &BPE::token_to_id);
}
