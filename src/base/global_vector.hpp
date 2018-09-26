/* ************************************************************************
 * Copyright (c) 2018 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#ifndef ROCALUTION_GLOBAL_VECTOR_HPP_
#define ROCALUTION_GLOBAL_VECTOR_HPP_

#include "../utils/types.hpp"
#include "vector.hpp"
#include "parallel_manager.hpp"

namespace rocalution {

template <typename ValueType>
class LocalVector;
template <typename ValueType>
class LocalMatrix;
template <typename ValueType>
class GlobalMatrix;
struct MRequest;

// Global vector
template <typename ValueType>
class GlobalVector : public Vector<ValueType>
{
    public:
    GlobalVector();
    GlobalVector(const ParallelManager& pm);
    virtual ~GlobalVector();

    virtual void MoveToAccelerator(void);
    virtual void MoveToHost(void);

    virtual void Info(void) const;
    virtual bool Check(void) const;

    virtual IndexType2 GetSize(void) const;
    virtual int GetLocalSize(void) const;
    virtual int GetGhostSize(void) const;

    const LocalVector<ValueType>& GetInterior() const;
    LocalVector<ValueType>& GetInterior();
    const LocalVector<ValueType>& GetGhost() const;

    virtual void Allocate(std::string name, IndexType2 size);
    virtual void Clear(void);

    void SetParallelManager(const ParallelManager& pm);

    virtual void Zeros(void);
    virtual void Ones(void);
    virtual void SetValues(ValueType val);
    virtual void SetRandomUniform(unsigned long long seed,
                                  ValueType a = static_cast<ValueType>(-1),
                                  ValueType b = static_cast<ValueType>(1));
    virtual void SetRandomNormal(unsigned long long seed,
                                 ValueType mean = static_cast<ValueType>(0),
                                 ValueType var  = static_cast<ValueType>(1));
    void CloneFrom(const GlobalVector<ValueType>& src);

    // Accessing operator - only for host data
    ValueType& operator[](int i);
    const ValueType& operator[](int i) const;

    void SetDataPtr(ValueType** ptr, std::string name, IndexType2 size);
    void LeaveDataPtr(ValueType** ptr);

    virtual void CopyFrom(const GlobalVector<ValueType>& src);
    virtual void ReadFileASCII(const std::string filename);
    virtual void WriteFileASCII(const std::string filename) const;
    virtual void ReadFileBinary(const std::string filename);
    virtual void WriteFileBinary(const std::string filename) const;

    // this = this + alpha*x
    virtual void AddScale(const GlobalVector<ValueType>& x, ValueType alpha);
    // this = alpha*this + x
    virtual void ScaleAdd(ValueType alpha, const GlobalVector<ValueType>& x);
    // this = alpha*this + x*beta + y*gamma
    virtual void ScaleAdd2(ValueType alpha,
                           const GlobalVector<ValueType>& x,
                           ValueType beta,
                           const GlobalVector<ValueType>& y,
                           ValueType gamma);
    // this = alpha * this + beta * x
    virtual void ScaleAddScale(ValueType alpha, const GlobalVector<ValueType>& x, ValueType beta);
    // this = alpha*this
    virtual void Scale(ValueType alpha);
    // this^T x
    virtual ValueType Dot(const GlobalVector<ValueType>& x) const;
    // this^T x
    virtual ValueType DotNonConj(const GlobalVector<ValueType>& x) const;
    // sqrt(this^T this)
    virtual ValueType Norm(void) const;
    // reduce
    virtual ValueType Reduce(void) const;
    // L1 norm, sum(|this|)
    virtual ValueType Asum(void) const;
    // Amax, max(|this|)
    virtual int Amax(ValueType& value) const;
    // point-wise multiplication
    virtual void PointWiseMult(const GlobalVector<ValueType>& x);
    virtual void PointWiseMult(const GlobalVector<ValueType>& x, const GlobalVector<ValueType>& y);

    virtual void Power(double power);

    // Restriction operator based on restriction mapping vector
    void Restriction(const GlobalVector<ValueType>& vec_fine, const LocalVector<int>& map);

    // Prolongation operator based on restriction(!) mapping vector
    void Prolongation(const GlobalVector<ValueType>& vec_coarse, const LocalVector<int>& map);

    protected:
    virtual bool is_host(void) const;
    virtual bool is_accel(void) const;

    void UpdateGhostValuesAsync(const GlobalVector<ValueType>& in);
    void UpdateGhostValuesSync(void);

    private:
    MRequest* recv_event_;
    MRequest* send_event_;

    ValueType* recv_boundary_;
    ValueType* send_boundary_;

    LocalVector<ValueType> vector_interior_;
    LocalVector<ValueType> vector_ghost_;

    friend class LocalMatrix<ValueType>;
    friend class GlobalMatrix<ValueType>;

    friend class BaseRocalution<ValueType>;
};

} // namespace rocalution

#endif // ROCALUTION_GLOBAL_VECTOR_HPP_
