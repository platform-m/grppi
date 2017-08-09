/**
* @version		GrPPI v0.3
* @copyright		Copyright (C) 2017 Universidad Carlos III de Madrid. All rights reserved.
* @license		GNU/GPL, see LICENSE.txt
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License in LICENSE.txt
* also available in <http://www.gnu.org/licenses/gpl.html>.
*
* See COPYRIGHT.txt for copyright notices and details.
*/

#ifndef GRPPI_SEQ_SEQUENTIAL_EXECUTION_H
#define GRPPI_SEQ_SEQUENTIAL_EXECUTION_H

#include "../common/iterator.h"

#include <type_traits>
#include <tuple>


namespace grppi {

/**
\brief Sequential execution policy.
*/
class sequential_execution {

public:

  /// \brief Default constructor.
  sequential_execution() {}

  /**
  \brief Set number of grppi threads.
  \note Setting concurrency degree is ignored for sequential execution.
  */
  void set_concurrency_degree(int n) const noexcept {}

  /**
  \brief Get number of grppi trheads.
  \note Getting concurrency degree is always 1 for sequential execution.
  */
  int concurrency_degree() const noexcept { return 1; }

  /**
  \brief Enable ordering.
  \note Enabling ordering of sequential execution is always ignored.
  */
  void enable_ordering() const noexcept {}

  /**
  \brief Disable ordering.
  \note Disabling ordering of sequential execution is always ignored.
  */
  void disable_ordering() const noexcept {}

  /**
  \brief Is execution ordered.
  \note Sequential execution is always ordered.
  */
  bool is_ordered() const noexcept { return true; }

  /**
  \brief Applies a trasnformation to multiple sequences leaving the result in
  another sequence.
  \tparam InputIterators Iterator types for input sequences.
  \tparam OutputIterator Iterator type for the output sequence.
  \tparam Transformer Callable object type for the transformation.
  \param firsts Tuple of iterators to input sequences.
  \param first_out Iterator to the output sequence.
  \param sequence_size Size of the input sequences.
  \param transform_op Transformation callable object.
  \pre For every I iterators in the range 
       `[get<I>(firsts), next(get<I>(firsts),sequence_size))` are valid.
  \pre Iterators in the range `[first_out, next(first_out,sequence_size)]` are valid.
  */
  template <typename ... InputIterators, typename OutputIterator, 
            typename Transformer>
  void apply_map(std::tuple<InputIterators...> firsts,
      OutputIterator first_out, 
      std::size_t sequence_size, Transformer transform_op) const;
  
  /**
  \brief Applies a reduction to a sequence of data items. 
  \tparam InputIterator Iterator type for the input sequence.
  \tparam Identity Type for the identity value.
  \tparam Combiner Callable object type for the combination.
  \param first Iterator to the first element of the sequence.
  \param last Iterator to one past the end of the sequence.
  \param identity Identity value for the reduction.
  \param combine_op Combination callable object.
  \pre Iterators in the range `[first,last)` are valid. 
  \return The reduction result
  */
  template <typename InputIterator, typename Identity, typename Combiner>
  auto reduce(InputIterator first, InputIterator last,
              Identity && identity,
              Combiner && combine_op) const;
};

template <typename ... InputIterators, typename OutputIterator,
          typename Transformer>
void sequential_execution::apply_map(
    std::tuple<InputIterators...> firsts,
    OutputIterator first_out, 
    std::size_t sequence_size, Transformer transform_op) const
{
  const auto last = std::next(std::get<0>(firsts), sequence_size);
  while (std::get<0>(firsts) != last) {
    *first_out++ = apply_iterators_increment(transform_op, firsts);
  }
}

template <typename InputIterator, typename Identity, typename Combiner>
auto sequential_execution::reduce(
    InputIterator first, InputIterator last,
    Identity && identity,
    Combiner && combine_op) const
{
  auto result{identity};
  while (first != last) {
    result = combine_op(result, *first++);
  }
  return result;
}

/// Determine if a type is a sequential execution policy.
template <typename E>
constexpr bool is_sequential_execution() {
  return std::is_same<E, sequential_execution>::value;
}

template <typename E>
constexpr bool is_supported();

template <>
constexpr bool is_supported<sequential_execution>() {
  return true;
}

} // end namespace grppi

#endif
