// Distributed under the MIT License.
// See LICENSE.txt for details.

/// \file
/// Defines classes representing tensor indices

#pragma once

#include <ostream>

#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

/// \ingroup Tensor
/// Whether a \ref SpacetimeIndex "TensorIndexType" is covariant or
/// contravariant
enum class UpLo {
  /// Contravariant, or Upper index
  Up,
  /// Covariant, or Lower index
  Lo
};

/// \cond HIDDEN_SYMBOLS
inline std::ostream& operator<<(std::ostream& os, const UpLo& ul) {
  return os << (ul == UpLo::Up ? "Up"s : "Lo"s);
}
/// \endcond

/// \ingroup Tensor
/// Indicates the ::Frame that a \ref SpacetimeIndex "TensorIndexType"
/// is in.
namespace Frame {
struct Logical {};
struct Grid {};
struct Inertial {};
struct Distorted {};
}  // namespace Frame

/// \cond HIDDEN_SYMBOLS
inline std::ostream& operator<<(std::ostream& os,
                                const Frame::Logical& /*meta*/) {
  return os << "Logical";
}
inline std::ostream& operator<<(std::ostream& os, const Frame::Grid& /*meta*/) {
  return os << "Grid";
}
inline std::ostream& operator<<(std::ostream& os,
                                const Frame::Inertial& /*meta*/) {
  return os << "Inertial";
}
inline std::ostream& operator<<(std::ostream& os,
                                const Frame::Distorted& /*meta*/) {
  return os << "Distorted";
}
/// \endcond

/// \ingroup Tensor
/// Indicates whether the \ref SpacetimeIndex "TensorIndexType" is
/// Spatial or Spacetime
enum class IndexType : char {
  /// The \ref SpatialIndex "TensorIndexType" is purely spatial
  Spatial,
  /// The \ref SpacetimeIndex "TensorIndexType" is a spacetime index
  Spacetime
};

/// \cond HIDDEN_SYMBOLS
inline std::ostream& operator<<(std::ostream& os, const IndexType& index_type) {
  return os << (index_type == IndexType::Spatial ? "Spatial" : "Spacetime");
}
/// \endcond

namespace Tensor_detail {
/// \ingroup Tensor
/// A ::TensorIndexType holds information about what type of index an index of a
/// Tensor is. It holds the information about the number of spatial dimensions,
/// whether the index is covariant or contravariant (::UpLo), the ::Frame the
/// index is in, and whether the Index is Spatial or Spacetime.
/// \tparam SpatialDim the spatial dimensionality of the TensorIndex
/// \tparam Ul either UpLo::Up or UpLo::Lo for contra or covariant
/// \tparam Fr the Frame the TensorIndex is in
/// \tparam Index either IndexType::Spatial or IndexType::Spacetime
template <size_t SpatialDim, UpLo Ul, typename Fr, IndexType Index>
struct TensorIndexType {
  static_assert(SpatialDim > 0,
                "Cannot have a spatial dimensionality less than 1 (one) in a "
                "TensorIndexType");
  using value_type = decltype(SpatialDim);
  static constexpr value_type dim =
      Index == IndexType::Spatial ? SpatialDim
                                  : SpatialDim + static_cast<value_type>(1);
  // value is here just so that some generic metafunctions can retrieve the dim
  // easily
  static constexpr value_type value = dim;
  static constexpr UpLo ul = Ul;
  using Frame = Fr;
  static constexpr IndexType index_type = Index;
};
}  // namespace Tensor_detail

/// \ingroup Tensor
/// A SpatialIndex holds information about the number of spatial
/// dimensions, whether the index is covariant or contravariant (::UpLo), and
/// the ::Frame the index is in.
/// \tparam SpatialDim the spatial dimensionality of the \ref
/// SpatialIndex "TensorIndexType"
/// \tparam Ul either UpLo::Up or UpLo::Lo for contra or covariant
/// \tparam Fr the ::Frame the \ref SpatialIndex "TensorIndexType"
/// is in
template <size_t SpatialDim, UpLo Ul, typename Fr>
using SpatialIndex =
    Tensor_detail::TensorIndexType<SpatialDim, Ul, Fr, IndexType::Spatial>;

/// \ingroup Tensor
/// A SpacetimeIndex holds information about the number of spatial
/// dimensions, whether the index is covariant or contravariant (::UpLo), and
/// the ::Frame the index is in.
///
/// \tparam SpatialDim the spatial dimensionality of the \ref
/// SpacetimeIndex "TensorIndexType"
/// \tparam Ul either UpLo::Up or UpLo::Lo for contra or covariant
/// \tparam Fr the ::Frame the \ref SpacetimeIndex "TensorIndexType"
/// is in
template <size_t SpatialDim, UpLo Ul, typename Fr>
using SpacetimeIndex =
    Tensor_detail::TensorIndexType<SpatialDim, Ul, Fr, IndexType::Spacetime>;

namespace tt {
// @{
/// \ingroup Tensor TypeTraits
/// Inherits from std::true_type if T is a \ref SpacetimeIndex
/// "TensorIndexType"
/// \tparam T the type to check
template <typename T>
struct is_tensor_index_type : std::false_type {};
/// \cond HIDDEN_SYMBOLS
template <size_t SpatialDim, UpLo Ul, typename Fr, IndexType Index>
struct is_tensor_index_type<
    Tensor_detail::TensorIndexType<SpatialDim, Ul, Fr, Index>>
    : std::true_type {};
/// \endcond
/// \see is_tensor_index_type
template <typename T>
using is_tensor_index_type_t = typename is_tensor_index_type<T>::type;
// @}
}  // namespace tt

/// \ingroup Tensor
/// Change the \ref SpacetimeIndex "TensorIndexType" to be covariant
/// if it's contravariant and vice-versa
///
/// Here is an example of how to use ::change_index_up_lo
/// \snippet Tensor.cpp change_up_lo
///
/// \tparam Index the \ref SpacetimeIndex "TensorIndexType" to change
template <typename Index>
using change_index_up_lo = Tensor_detail::TensorIndexType<
    Index::index_type == IndexType::Spatial ? Index::value : Index::value - 1,
    Index::ul == UpLo::Up ? UpLo::Lo : UpLo::Up, typename Index::Frame,
    Index::index_type>;

template <typename... Ts>
using index_list = typelist<Ts...>;