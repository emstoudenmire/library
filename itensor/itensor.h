//
// Distributed under the ITensor Library License, Version 1.2
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_ITENSOR_H
#define __ITENSOR_ITENSOR_H
#include "itensor/detail/algs.h"
#include "itensor/itdata/applyfunc.h"
#include "itensor/indexset.h"
#define REGISTER_ITDATA_HEADER_FILES
#include "itensor/itdata/storage_types.h"
#include "itensor/tensor/mat.h"

namespace itensor {


class ITensor
    {
    public:
    using index_type = Index;
    using range_type = RangeT<Index>;
    using size_type = typename range_type::size_type;
    using storage_ptr = PData;
    using const_storage_ptr = CPData;
    using scale_type = LogNum;
    private:
    IndexSet is_;
    mutable storage_ptr store_;
    IF_USESCALE(scale_type scale_;)
    public:

    //
    // Constructors
    //

    //Default constructed tensor will evaluate to false in boolean context
    ITensor() { }

    //Construct n-index tensor, all elements set to zero
    //Usage: ITensor(i1,i2,i3,...)
    template <typename... Indices>
    explicit
    ITensor(Index  const& i1,
            Indices const&... i2etc);

    explicit
    ITensor(std::vector<Index> const& inds);

    template<size_t N> 
    explicit
    ITensor(std::array<Index,N> const& inds);

    ITensor(std::initializer_list<Index> inds);

    //Construct order 0 tensor (scalar), value set to val
    //If val.imag()==0, storage will be Real
    explicit
    ITensor(Cplx val);

    //
    // Accessor Methods
    //

    //Tensor order (number of indices)
    int 
    order() const { return is_.order(); }

    // Deprecated
    int 
    r() const { return this->order(); }

    //Access index set
    IndexSet const&
    inds() const { return is_; }

    //Access index
    Index const&
    index(size_type I) const { return is_.index(I); }

    //evaluates to false if default constructed
    explicit operator bool() const { return bool(is_) || bool(store_); }

    template <typename... IndexVals>
    Real
    elt(IndexVals&&... ivs) const;

    template <typename IV, typename... IVs>
    auto
    eltC(IV const& iv1, IVs&&... ivs) const
         -> stdx::if_compiles_return<Cplx,decltype(iv1.index),decltype(iv1.val)>;

    template <typename Int, typename... Ints>
    auto
    eltC(Int iv1, Ints... ivs) const
        -> stdx::enable_if_t<std::is_integral<Int>::value 
                          && stdx::and_<std::is_integral<Ints>...>::value,Cplx>;

    Cplx
    eltC() const;

    Cplx
    eltC(std::vector<IndexVal> const& ivs) const;

    template<typename Int>
    auto
    eltC(std::vector<Int> const& ints) const
        -> stdx::enable_if_t<std::is_integral<Int>::value,Cplx>;

    //Set element at location given by collection
    //of IndexVals or IQIndexVals. Will not switch storage
    //from Real to Complex unless val.imag()!=0 
    template<typename IV, typename... VArgs>
    auto
    set(IV const& iv1, VArgs&&... ivs)
        -> stdx::if_compiles_return<void,decltype(iv1.index),decltype(iv1.val)>;

    template<typename Int, typename... VArgs>
    auto
    set(Int iv1, VArgs&&... ivs)
        -> stdx::enable_if_t<std::is_integral<Int>::value,void>;

    void
    set(Cplx val);

    void
    set(std::vector<IndexVal> const& ivs, Cplx val);

    void
    set(std::vector<int> const& ivs, Cplx val);

    ITensor&
    randomize(Args const& args = Args::global());

    //
    // Index Prime Level Methods
    //

    template<typename... VarArgs>
    ITensor& 
    setPrime(VarArgs&&... vargs)
        { is_.setPrime(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor& 
    noPrime(VarArgs&&... vargs)
        { is_.noPrime(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor& 
    prime(VarArgs&&... vargs)
        { is_.prime(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor& 
    mapPrime(VarArgs&&... vargs)
        { is_.mapPrime(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor& 
    swapPrime(VarArgs&&... vargs)
        { is_.swapPrime(std::forward<VarArgs>(vargs)...); return *this; }

    // Deprecations
    template<typename... VarArgs>
    ITensor& 
    noprime(VarArgs&&... vargs)
        { Error(".noprime() is deprecated, use .noPrime() instead"); return *this; }

    //
    // Index Tag Methods
    //

    template<typename... VarArgs>
    ITensor&
    replaceTags(VarArgs&&... vargs)
        { is_.replaceTags(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor&
    setTags(VarArgs&&... vargs)
        { is_.setTags(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor&
    addTags(VarArgs&&... vargs)
        { is_.addTags(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor&
    removeTags(VarArgs&&... vargs)
        { is_.removeTags(std::forward<VarArgs>(vargs)...); return *this; }

    template<typename... VarArgs>
    ITensor&
    swapTags(VarArgs&&... vargs)
        { is_.swapTags(std::forward<VarArgs>(vargs)...); return *this; }

    //
    // Element Transformation Methods
    //

    //Set all elements to z. If z.imag()==0
    //(such as if z is automatically converted from a Real)
    //then storage will be real only.
    ITensor&
    fill(Complex z);

    //Call a function of the form f()->val once
    //for each element, assign result to each element.
    template <typename Func>
    ITensor&
    generate(Func&& f);

    //Apply a function of the form f(x)->y
    //to each element x, replacing it with y
    template <typename Func>
    ITensor&
    apply(Func&& f);

    //Apply a function of the form f(x)->void
    //to each element x.
    template <typename Func>
    const ITensor&
    visit(Func&& f) const;

    //
    // Complex number methods
    //

    //Take complex conjugate of all elements
    ITensor&
    conj();

    ITensor&
    dag();

    //Replace data with real part
    ITensor&
    takeReal();

    //Replace data with imaginary part
    ITensor&
    takeImag();

    //
    // Operators
    //

    //Contracting product
    //All matching Index pairs automatically contracted
    //Cji = \sum_{k,l} Akjl * Blki
    ITensor&
    operator*=(ITensor const& other);

    //Tensor addition and subtraction
    //Summands must have same Indices, in any order
    //Cijk = Aijk + Bkij
    ITensor& 
    operator+=(ITensor const& other);
    ITensor& 
    operator-=(ITensor const& other);

#ifdef USESCALE
    //Multiplication by real scalar
    ITensor&
    operator*=(Real fac) { scale_ *= fac; return *this; }

    //Division by real scalar
    ITensor&
    operator/=(Real fac) { scale_ /= fac; return *this; }
#else
    //Multiplication by real scalar
    ITensor&
    operator*=(Real fac);

    //Division by real scalar
    ITensor&
    operator/=(Real fac);
#endif

    //Multiplication by complex scalar
    ITensor&
    operator*=(Cplx z);

    //Division by complex scalar
    ITensor&
    operator/=(Cplx z) { return operator*=(1./z); }

    //Negation
    ITensor
    operator-() const;

    //Non-contracting product
    //All matching Index pairs automatically merged
    //Ciik = Aij * Bjk
    ITensor&
    operator/=(ITensor const& other);

    //template<typename... Indxs>
    //ITensor&
    //permute(index_type const& ind1, Indxs const&... inds);

    template<typename... Indxs>
    auto 
    permute(Index const& ind1, Indxs const&... inds)
            -> stdx::enable_if_t<not stdx::and_<std::is_same<Index, Indxs>...>::value,ITensor&>;

    template <typename... Indxs>
    auto 
    permute(Index const& ind1, Indxs const&... inds)
            -> stdx::enable_if_t<stdx::and_<std::is_same<Index, Indxs>...>::value,ITensor&>;

    template<typename... Indxs>
    ITensor&
    permute(std::string const& dots, Indxs const&... inds);

    ITensor&
    permute(IndexSet const& iset);

    //
    // Read from and write to streams
    //

    void
    read(std::istream& s);

    void
    write(std::ostream& s) const;


    //
    // Developer / advanced methods
    //
    // The following methods should not
    // be needed for most user code.
    //

    template <class StorageType>
    ITensor(IndexSet iset,
            StorageType&& store,
            scale_type const& scale = LogNum{1.});

    ITensor(IndexSet iset,
            storage_ptr&& pstore,
            scale_type const& scale = LogNum{1.});

    //Provide indices from IndexSet
    explicit
    ITensor(IndexSet const& is);

    storage_ptr&
    store() { return store_; }

    const_storage_ptr
    store() const { return const_storage_ptr(store_); }

    void
    swap(ITensor & other);
    
#ifdef USESCALE

    scale_type const&
    scale() const { return scale_; }

    scale_type&
    scale() { return scale_; }
    
    void 
    scaleTo(scale_type const& newscale);
    
    void 
    scaleTo(Real newscale);

#else //not using scale, default case:

    scale_type
    scale() const { return scale_type(1.); }

    //scale_type&
    //scale() { return scale_; }
    
    void 
    scaleTo(scale_type const& newscale) { }
    
    void 
    scaleTo(Real newscale) { }

#endif

    //
    // Deprecated methods
    //

    template <typename... IndexVals>
    Real
    real(IndexVals&&... ivs) const;

    template <typename... IndexVals>
    Cplx
    cplx(IndexVals&&... ivs) const;

    }; // class ITensor

//
// ITensor special constructor functions
//

// Makes a tensor with element specified by IndexVals
// set to 1.0, all other elements zero
template <typename IVal, typename... IVals>
ITensor
setElt(IVal  const& iv1, 
       IVals const&... rest);

// Get ITensor values
template <typename... VarArgs>
Real
elt(ITensor A,
    VarArgs&&... vargs);

template <typename... VarArgs>
Cplx
eltC(ITensor A,
     VarArgs&&... vargs);

//
// ITensor prime level functions
//

template<typename... VarArgs>
ITensor
setPrime(ITensor A, 
         VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
prime(ITensor A, 
      VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
noPrime(ITensor A, 
        VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
mapPrime(ITensor A, 
         VarArgs&&... vargs);

//
//Return copy of a tensor with primeLevels plev1 and plev2 swapped
//
//For example, if T has indices i,i' (like a matrix or a site
//operator) then swapPrime(T,0,1) will have indices i',i 
//i.e. the transpose of T.
//
template<typename... VarArgs>
ITensor
swapPrime(ITensor A,
          VarArgs&&... vargs);

//
// ITensor tag functions
//

template<typename... VarArgs>
ITensor
replaceTags(ITensor A,
            VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
setTags(ITensor A,
        VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
addTags(ITensor A,
        VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
removeTags(ITensor A,
           VarArgs&&... vargs);

template<typename... VarArgs>
ITensor
swapTags(ITensor A,
         VarArgs&&... vargs);

bool
hasIndex(ITensor const& T, Index const& I);

//template<typename Cond>
//Index
//findIndex(ITensor const& T, 
//          Cond && cond);

Index
findIndex(ITensor const& T,
          TagSet const& tsmatch, 
          int plmatch = -1);

Index
findIndexExact(ITensor const& T,
               TagSet const& tsmatch, 
               int plmatch = -1);

//Find index of tensor A (optionally having tags ts)
//which is shared with tensor B
Index
commonIndex(ITensor const& A, 
            ITensor const& B, 
            TagSet const& ts = TagSet(All));


//Find index of tensor A (of optional type t) 
//which is NOT shared by tensor B
Index
uniqueIndex(ITensor const& A, 
            ITensor const& B, 
            TagSet const& ts = TagSet(All));

template<typename... Tensors> 
Index
uniqueIndex(ITensor const& A, 
            ITensor const& T1,
            ITensor const& T2,
            Tensors const&... Tens);

//Apply x = f(x) for each element x of T
//and return the resulting tensor
template<typename F>
ITensor
apply(ITensor T, F&& f);

ITensor inline
realPart(ITensor T) { T.takeReal(); return T; }

ITensor inline
imagPart(ITensor T) { T.takeImag(); return T; }

bool
isComplex(ITensor const& T);

bool
isReal(ITensor const& T);

//return number of indices of T
long
order(ITensor const& T);

// Deprecated, same as order
long
rank(ITensor const& T);

//Compute the norm of an ITensor.
//Thinking of elements as a vector, equivalent to sqrt(v*v).
//Result is equivalent to sqrt((T*T).elt()) 
//(and similar for complex case) but computed more efficiently
Real
norm(ITensor const& T);

void
randomize(ITensor & T, Args const& args = Args::global());

ITensor
random(ITensor T, Args const& args = Args::global());

ITensor
conj(ITensor T);

ITensor
dag(ITensor T);

Real
sumels(ITensor const& t);

Cplx
sumelsC(ITensor const& t);

template<typename... Inds>
ITensor
replaceInds(ITensor const& cT, 
            Index o1, Index n1, 
            Inds... inds);

//This is deprecated, use replaceInds() instead
template<typename... Inds>
ITensor
reindex(ITensor const& cT, 
        Index o1, Index n1, 
        Inds... inds);


//
// Given Tensors which represent operator matrices
// (e.g. A(site1',site1), B(site1',site1) )
// multiply them, automatically adjusting primeLevels
// so that result is again an operator matrix C(site1',site1)
//
//              s'  t'
//  s'  t'      |   |
//  |   |       [-A-]
//  [-C-]  =    |   |
//  |   |       [-B-]
//  s   t       |   |
//              s   t
//
// (here s and t are indices of type Site)
//
ITensor
multSiteOps(ITensor A, ITensor const& B);

ITensor
combiner(IndexSet const& inds, Args const& args = Global::args());

ITensor
combiner(std::vector<Index> const& inds, Args const& args = Global::args());

ITensor
combiner(std::initializer_list<Index> inds, Args const& args = Global::args());

template <size_t N>
ITensor
combiner(std::array<Index,N> inds, Args const& args = Global::args())
    {
    return combiner(IndexSet(inds),args);
    }

template<typename... Inds>
ITensor
combiner(Index const& i1, 
         Inds const&... inds)
    {
    return combiner(std::vector<Index>{i1,inds...});
    }

Index
combinedIndex(ITensor const& C);


//Construct diagonal ITensor with diagonal 
//elements set to 1.0
template<typename... Inds>
ITensor
delta(Index const& i1,
      Inds const&... inds);

//Construct diagonal ITensor,
//diagonal elements given by container C
//(Uses elements C.begin() up to C.end())
template<typename Container, 
         typename... Inds,
         class = stdx::enable_if_t<stdx::containerOf<Real,Container>::value
                                || stdx::containerOf<Cplx,Container>::value> >
ITensor
diagITensor(Container const& C,
            Index const& i1,
            Inds&&... inds);

//Depecrated
template<typename Container, 
         typename... Inds,
         class = stdx::enable_if_t<stdx::containerOf<Real,Container>::value
                                || stdx::containerOf<Cplx,Container>::value> >
ITensor
diagTensor(Container const& C,
           Index const& i1,
           Inds&&... inds);

template <typename... Inds>
ITensor
randomITensor(Index const& i1, Inds&&... inds);
template <typename... Inds>
ITensor
randomITensorC(Index const& i1, Inds&&... inds);
ITensor
randomITensor(IndexSet const& inds);

//Depecrated
template <typename... Inds>
ITensor
randomTensor(Index const& i1, Inds&&... inds);
template <typename... Inds>
ITensor
randomTensorC(Index const& i1, Inds&&... inds);
ITensor
randomTensor(IndexSet const& inds);

template <typename... Inds>
ITensor
randomITensor(QN q, Index const& i1, Inds&&... inds);
template <typename... Inds>
ITensor
randomITensorC(QN q, Index const& i1, Inds&&... inds);
ITensor
randomITensor(QN q, IndexSet const& inds, Args const& args = Args::global());

//Deprecated
template <typename... Inds>
ITensor
randomTensor(QN q, Index const& i1, Inds&&... inds);
template <typename... Inds>
ITensor
randomTensorC(QN q, Index const& i1, Inds&&... inds);
ITensor
randomTensor(QN q, IndexSet const& inds, Args const& args = Args::global());

ITensor
matrixITensor(Matrix && M, Index const& i1, Index const& i2);
ITensor
matrixITensor(Matrix const& M, Index const& i1, Index const& i2);
ITensor
matrixITensor(CMatrix && M, Index const& i1, Index const& i2);
ITensor
matrixITensor(CMatrix const& M, Index const& i1, Index const& i2);

//Deprecated
ITensor
matrixTensor(Matrix && M, Index const& i1, Index const& i2);
ITensor
matrixTensor(Matrix const& M, Index const& i1, Index const& i2);
ITensor
matrixTensor(CMatrix && M, Index const& i1, Index const& i2);
ITensor
matrixTensor(CMatrix const& M, Index const& i1, Index const& i2);

//
// QN ITensor related functions
//

QN
div(ITensor const& T);

//flux is an alias for div
QN
flux(ITensor const& T);

bool
hasQNs(ITensor const& T);

ITensor
removeQNs(ITensor T);

template<typename V>
TenRef<Range,V>
getBlock(ITensor & T, IntArray block_ind);

std::ostream& 
operator<<(std::ostream & s, ITensor const& T);

} //namespace itensor

#include "itensor_impl.h"


#endif
