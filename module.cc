// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause
// License.

// Include the defined classes that are to be exported to python
#include <pybind11/pybind11.h>
#include "EvaluatorPairHertzian.h"
#include "EvaluatorPairMLJ.h"
#include "EvaluatorPairLJLow.h"
#include "EvaluatorPairWLJ.h"
#include "EvaluatorPairDipoleDipole.h"
// #include "EvaluatorPairSpring.h"
// #include "HPFPotentialPair.h"
#include "hoomd/md/PotentialPair.h"

#ifdef ENABLE_HIP
#include "hoomd/md/PotentialPairGPU.h"
#endif

namespace hoomd
    {
namespace md
    {

// specify the python module. Note that the name must explicitly match
// the PROJECT() name provided in CMakeLists (with an underscore in
// front)
PYBIND11_MODULE(_pair_plugin, m)
    {
    detail::export_PotentialPair<EvaluatorPairMLJ>(m, "PotentialPairMLJ");
    detail::export_PotentialPair<EvaluatorPairWLJ>(m, "PotentialPairWLJ");
    detail::export_PotentialPair<EvaluatorPairHertzian>(m, "PotentialPairHertzian");
    detail::export_PotentialPair<EvaluatorPairDipoleDipole>(m, "PotentialPairDipoleDipole");
    detail::export_PotentialPair<EvaluatorPairLJLow>(m, "PotentialPairLJLow");
    // detail::export_HPFPotentialPair<EvaluatorPairHarmSpring>(m, "PotentialPairHPF");
    // detail::export_GranularPotentialPair<EvaluatorPairHarmSpring>(m, "PotentialPairGranular");
#ifdef ENABLE_HIP
    detail::export_PotentialPairGPU<EvaluatorPairMLJ>(m, "PotentialPairMLJGPU");
    detail::export_PotentialPairGPU<EvaluatorPairWLJ>(m, "PotentialPairWLJGPU");
    detail::export_PotentialPairGPU<EvaluatorPairHertzian>(m, "PotentialPairHertzianGPU");
    detail::export_PotentialPairGPU<EvaluatorPairDipoleDipole>(m, "PotentialPairDipoleDipoleGPU");
    detail::export_PotentialPairGPU<EvaluatorPairLJLow>(m, "PotentialPairLJLowGPU");
    // TODO, write GPU implementation
    // detail::export_FrictionPotentialPairGPU<EvaluatorPairFrictionLJ>(m,
    // "PotentialPairFrictionLJGPU");
#endif
    }

    } // end namespace md
    } // end namespace hoomd
