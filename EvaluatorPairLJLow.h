// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause
// License.

// Maintainer: joaander

#ifndef __PAIR_EVALUATOR_LJLOW_H__
#define __PAIR_EVALUATOR_LJLOW_H__

#ifndef __HIPCC__
#include <string>
#endif

// #include "hoomd/md/EvaluatorPairLJ.h"
#include "hoomd/HOOMDMath.h"

/*! \file EvaluatorPairLJLow.h
    \brief Defines the pair evaluator class for the modified LJ potential
    \details Modified version of the LJ potential
*/

// need to declare these class methods with __device__ qualifiers when building
// in nvcc DEVICE is __host__ __device__ when included in nvcc and blank when
// included into the host compiler
#ifdef __HIPCC__
#define DEVICE __device__
#define HOSTDEVICE __host__ __device__
#else
#define DEVICE
#define HOSTDEVICE
#endif

namespace hoomd
    {
namespace md
    {
//! Class for evaluating the modified LJ pair potential
/*! <b>Original</b>
    <b>General Overview</b>

    EvaluatorPairLJ is a low level computation class that computes the
   LJ pair potential V(r). As the standard MD potential, it also serves
   as a well documented example of how to write additional pair
   potentials. "Standard" pair potentials in hoomd are all handled via
   the template class PotentialPair. PotentialPair takes a potential
   evaluator as a template argument. In this way, all the complicated
   data management and other details of computing the pair force and
   potential on every single particle is only written once in the
   template class and the difference V(r) potentials that can be
   calculated are simply handled with various evaluator classes.
   Template instantiation is equivalent to inlining code, so there is no
   performance loss.

    In hoomd, a "standard" pair potential is defined as V(rsq, rcutsq,
   params, di, dj, qi, qj), where rsq is the squared distance between
   the two particles, rcutsq is the cutoff radius at which the potential
   goes to 0, params is any number of per type-pair parameters, di, dj
   are the diameters of particles i and j, and qi, qj are the charges of
   particles i and j respectively.

    Diameter and charge are not always needed by a given pair evaluator,
   so it must provide the functions needsDiameter() and needsCharge()
   which return boolean values signifying if they need those quantities
   or not. A false return value notifies PotentialPair that it need not
   even load those values from memory, boosting performance.

    If needsDiameter() returns true, a setDiameter(ShortReal di, ShortReal dj)
   method will be called to set the two diameters. Similarly, if
   needsCharge() returns true, a setCharge(ShortReal qi, ShortReal qj) method
   will be called to set the two charges.

    All other arguments are common among all pair potentials and passed
   into the constructor. Coefficients are handled in a special way: the
   pair evaluator class (and PotentialPair) manage only a single
   parameter variable for each type pair. Pair potentials that need more
   than 1 parameter can specify that their param_type be a compound
   structure and reference that. For coalesced read performance on G200
   GPUs, it is highly recommended that param_type is one of the
   following types: ShortReal, ShortReal2, ShortReal4.

    The program flow will proceed like this: When a potential between a
   pair of particles is to be evaluated, a PairEvaluator is
   instantiated, passing the common parameters to the constructor and
   calling setDiameter() and/or setCharge() if need be. Then, the
   evalForceAndEnergy() method is called to evaluate the force and
   energy (more on that later). Thus, the evaluator must save all of the
   values it needs to compute the force and energy in member variables.

    evalForceAndEnergy() makes the necessary computations and sets the
   out parameters with the computed values. Specifically after the
   method completes, \a force_divr must be set to the value \f$
   -\frac{1}{r}\frac{\partial V}{\partial r}\f$ and \a pair_eng must be
   set to the value \f$ V(r) \f$ if \a energy_shift is false or \f$ V(r)
   - V(r_{\mathrm{cut}}) \f$ if \a energy_shift is true.

    A pair potential evaluator class is also used on the GPU. So all of
   its members must be declared with the DEVICE keyword before them to
   mark them
   __device__ when compiling in nvcc and blank otherwise. If any other
   code needs to diverge between the host and device (i.e., to use a
   special math function like __powf on the device), it can similarly be
   put inside an ifdef
   __HIPCC__ block.

    <b>LJ specifics</b>

    EvaluatorPairLJ evaluates the function:
    \f[ V_{\mathrm{LJ}}(r) = 4 \varepsilon \left[ \left(
   \frac{\sigma}{r} \right)^{12} - \left( \frac{\sigma}{r} \right)^{6}
   \right] \f] broken up as follows for efficiency \f[
   V_{\mathrm{LJ}}(r) = r^{-6} \cdot \left( 4 \varepsilon \sigma^{12}
   \cdot r^{-6} - 4 \varepsilon \sigma^{6} \right) \f] . Similarly, \f[
   -\frac{1}{r} \frac{\partial V_{\mathrm{LJ}}}{\partial r} = r^{-2}
   \cdot r^{-6} \cdot \left( 12 \cdot 4 \varepsilon \sigma^{12} \cdot
   r^{-6} - 6 \cdot 4 \varepsilon \sigma^{6} \right) \f]

    The LJ potential does not need diameter or charge. Two parameters
   are specified and stored in a ShortReal2. \a lj1 is placed in \a
   params.x and \a lj2 is in \a params.y.

    These are related to the standard lj parameters sigma and epsilon
   by:
    - \a lj1 = 4.0 * epsilon * pow(sigma,12.0)
    - \a lj2 = 4.0 * epsilon * pow(sigma,6.0);

    <b>Modifications</b>
    We modify the LJ potential slightly to instead evaluate the
   function: \f[ V_{\mathrm{LJ}}(r) = 4 \varepsilon \left[ \left(
   \frac{\sigma'}{r-\Delta} \right)^{12} - \left(
   \frac{\sigma'}{r-\Delta} \right)^{6} \right] \f] where \f[ \sigma' =
   \sigma - \frac{\Delta}{2^{1/6}} \f]

    This is similar to the LJ expand potential from LAMMPS, though

*/
class EvaluatorPairLJLow
    {
    public:
    //! Define the parameter type used by this pair potential evaluator
    struct param_type
        {
        ShortReal lj1;
        ShortReal lj2;

        DEVICE void load_shared(char*& ptr, unsigned int& available_bytes) { }

        HOSTDEVICE void allocate_shared(char*& ptr, unsigned int& available_bytes) const { }

#ifdef ENABLE_HIP
        //! Set CUDA memory hints
        void set_memory_hint() const
            {
            // default implementation does nothing
            }
#endif

#ifndef __HIPCC__
        param_type() : lj1(0), lj2(0) { }

        param_type(pybind11::dict v, bool managed = false)
            {
            auto sigma(v["sigma"].cast<ShortReal>());
            auto epsilon(v["epsilon"].cast<ShortReal>());
            lj1 = 4.0 * epsilon * pow(sigma, 12.0);
            lj2 = 4.0 * epsilon * pow(sigma, 6.0);
            }

        // this constructor facilitates unit testing
        param_type(ShortReal sigma, ShortReal epsilon, bool managed = false)
            {
            lj1 = 4.0 * epsilon * pow(sigma, 12.0);
            lj2 = 4.0 * epsilon * pow(sigma, 6.0);
            }

        pybind11::dict asDict()
            {
            pybind11::dict v;
            auto sigma6 = lj1 / lj2;
            v["sigma"] = pow(sigma6, 1. / 6.);
            v["epsilon"] = lj2 / (sigma6 * 4);
            return v;
            }
#endif
        }
#ifdef SINGLE_PRECISION
        __attribute__((aligned(8)));
#else
        __attribute__((aligned(16)));
#endif

    //! Constructs the pair potential evaluator
    /*! \param _rsq Squared distance between the particles
        \param _rcutsq Squared distance at which the potential goes to 0
        \param _params Per type pair parameters of this potential
    */
    DEVICE EvaluatorPairLJLow(ShortReal _rsq, ShortReal _rcutsq, const param_type& _params)
        : rsq(_rsq), rcutsq(_rcutsq), lj1(_params.lj1), lj2(_params.lj2)
        {
        }

    //! LJ doesn't use diameter
    DEVICE static bool needsDiameter()
        {
        return false;
        }
    //! Accept the optional diameter values
    /*! \param di Diameter of particle i
        \param dj Diameter of particle j
    */
    DEVICE void setDiameter(Scalar di, Scalar dj) { }

    //! LJ doesn't use charge
    DEVICE static bool needsCharge()
        {
        return false;
        }
    //! Accept the optional diameter values
    /*! \param qi Charge of particle i
        \param qj Charge of particle j
    */
    DEVICE void setCharge(Scalar qi, Scalar qj) { }

    //! Evaluate the force and energy
    /*! \param force_divr Output parameter to write the computed force
       divided by r. \param pair_eng Output parameter to write the
       computed pair energy \param energy_shift If true, the potential
       must be shifted so that V(r) is continuous at the cutoff \note
       There is no need to check if rsq < rcutsq in this method. Cutoff
       tests are performed in PotentialPair.

        \return True if they are evaluated or false if they are not
       because we are beyond the cutoff
    */
    DEVICE bool evalForceAndEnergy(Scalar& force_divr, Scalar& pair_eng, bool energy_shift)
        {
        // compute the force divided by r in force_divr
        if (rsq < rcutsq && lj1 != 0)
            {
            ShortReal r2inv = ShortReal(1.0) / rsq;
            ShortReal r6inv = r2inv * r2inv * r2inv;
            force_divr = r2inv * r6inv * (ShortReal(12.0) * lj1 * r6inv - ShortReal(6.0) * lj2);

            pair_eng = r6inv * (lj1 * r6inv - lj2);

            if (energy_shift)
                {
                ShortReal rcut2inv = ShortReal(1.0) / rcutsq;
                ShortReal rcut6inv = rcut2inv * rcut2inv * rcut2inv;
                pair_eng -= rcut6inv * (lj1 * rcut6inv - lj2);
                }
            return true;
            }
        else
            return false;
        }

    DEVICE Scalar evalPressureLRCIntegral()
        {
        return 0;
        }

    DEVICE Scalar evalEnergyLRCIntegral()
        {
        return 0;
        }

#ifndef __HIPCC__
    //! Get the name of this potential
    /*! \returns The potential name.
     */
    static std::string getName()
        {
        return std::string("ljlow");
        }

    std::string getShapeSpec() const
        {
        throw std::runtime_error("Shape definition not supported for this pair potential.");
        }
#endif

    protected:
    ShortReal rsq;    //!< Stored rsq from the constructor
    ShortReal rcutsq; //!< Stored rcutsq from the constructor
    ShortReal lj1;    //!< lj1 parameter extracted from the params passed to
                   //!< the constructor
    ShortReal lj2;    //!< lj2 parameter extracted from the params passed to
                   //!< the constructor
    // Add any additional fields
    };

    } // end namespace md
    } // end namespace hoomd

#endif // __PAIR_EVALUATOR_LJLOW_H__
