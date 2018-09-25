/* ************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 *
 * ************************************************************************ */

#ifndef ROCALUTION_SMOOTHED_AMG_HPP_
#define ROCALUTION_SMOOTHED_AMG_HPP_

#include "../solver.hpp"
#include "base_amg.hpp"

#include <vector>

namespace rocalution {

/** \ingroup solver_module
  * \class SAAMG
  * \brief Smoothed Aggregation Algebraic MultiGrid Method
  * \details
  * The Smoothed Aggregation Algebraic MultiGrid method is based on smoothed
  * aggregation based interpolation scheme, see "Vanek, P., Mandel, J., and Brezina,
  * M. Algebraic multigrid by smoothed aggregation for second and fourth order
  * elliptic problems. Computing 56, 3 (1996), 179–196" for details.
  *
  * \tparam OperatorType - can be LocalMatrix
  * \tparam VectorType - can be LocalVector
  * \tparam ValueType - can be float, double, std::complex<float> or std::complex<double>
  */
template <class OperatorType, class VectorType, typename ValueType>
class SAAMG : public BaseAMG<OperatorType, VectorType, ValueType>
{
    public:
    SAAMG();
    virtual ~SAAMG();

    virtual void Print(void) const;
    virtual void BuildSmoothers(void);

    /** \brief Set coupling strength */
    void SetCouplingStrength(ValueType eps);
    /** \brief Set the relaxation parameter */
    void SetInterpRelax(ValueType relax);

    virtual void ReBuildNumeric(void);

    protected:
    virtual void Aggregate_(const OperatorType& op,
                            Operator<ValueType>* pro,
                            Operator<ValueType>* res,
                            OperatorType* coarse);

    virtual void PrintStart_(void) const;
    virtual void PrintEnd_(void) const;

    private:
    /** \brief Coupling strength */
    ValueType eps_;

    /** \brief Relaxation parameter */
    ValueType relax_;
};

} // namespace rocalution

#endif // ROCALUTION_SMOOTHED_AMG_HPP_
