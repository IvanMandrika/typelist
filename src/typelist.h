#pragma once

#include <tuple>

namespace tl {
// Type List
template <typename... Types>
struct type_list {};

// Apply
template <template <typename...> typename F, typename List>
struct apply_impl;

template <template <typename...> typename F, template <typename...> typename List, typename... Types>
struct apply_impl<F, List<Types...>> {
  using type = F<Types...>;
};

template <template <typename...> typename F, typename List>
using apply = apply_impl<F, List>::type;

// Pop front
template <typename List>
struct pop_front_impl;

template <typename Head, template <typename...> typename List, typename... Types>
struct pop_front_impl<List<Head, Types...>> {
  using type = List<Types...>;
};

template <typename List>
using pop_front = pop_front_impl<List>::type;

// Front
template <typename List>
struct front_impl;

template <template <typename...> typename List, typename Head, typename... Types>
struct front_impl<List<Head, Types...>> {
  using type = Head;
};

template <typename List>
using front = front_impl<List>::type;

// Concatenate an arbitrary number of typelists

template <typename Type>
struct identity {
  using type = Type;
};

template <
    template <typename...>
    typename List1,
    template <typename...>
    typename List2,
    typename... Types1,
    typename... Types2>
identity<List1<Types1..., Types2...>> operator+(identity<List1<Types1...>>, identity<List2<Types2...>>);

template <typename... Lists>
using concat_fast = decltype((identity<Lists>{} + ... + identity<type_list<>>{}))::type;

// Conditional
template <bool B>
struct conditional_impl;

template <>
struct conditional_impl<true> {
  template <typename T, typename F>
  using type = T;
};

template <>
struct conditional_impl<false> {
  template <typename T, typename F>
  using type = F;
};

template <bool B, typename T, typename F>
using conditional = conditional_impl<B>::template type<T, F>;

// Filter
template <template <typename...> typename Pred, typename List>
struct filter_impl;

template <template <typename...> typename Pred, typename List>
using filter = filter_impl<Pred, List>::type;

template <template <typename...> typename Pred, template <typename...> typename List, typename... Types>
struct filter_impl<Pred, List<Types...>> {
  template <typename Type>
  using single = conditional<Pred<Type>::value, List<Type>, List<>>;

  using type = concat_fast<single<Types>...>;
};

template <template <typename...> typename Pred, template <typename...> typename List>
struct filter_impl<Pred, List<>> {
  using type = List<>;
};

// Index
template <std::size_t N>
using index = std::integral_constant<std::size_t, N>;

// Map Find
template <typename... Bases>
struct inherit : Bases... {};

template <typename Key, typename Map>
struct map_find_impl;

template <typename Key, typename... Lists>
struct map_find_impl<Key, type_list<Lists...>> {
  template <typename... Rest>
  static type_list<Key, Rest...> f(type_list<Key, Rest...>*);

  using derived = inherit<Lists...>;

  using type = decltype(f(static_cast<derived*>(nullptr)));
};

template <typename Key, typename Map>
using map_find = map_find_impl<Key, Map>::type;

// Second
template <typename List>
struct second_impl;

template <typename First, typename Second, template <typename...> typename List, typename... Rest>
struct second_impl<List<First, Second, Rest...>> {
  using type = Second;
};

template <typename List>
using second = second_impl<List>::type;

// Count
template <typename List>
struct count_impl;

template <template <typename...> typename List, typename... Types>
struct count_impl<List<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

template <typename List>
inline constexpr std::size_t count = count_impl<List>::value;

// Enumerate
template <typename List, typename Seq>
struct enumerate_impl;

template <template <typename...> class List, typename... Types, std::size_t... Idxs>
struct enumerate_impl<List<Types...>, std::index_sequence<Idxs...>> {
  using type = type_list<type_list<index<Idxs>, Types>...>;
};

template <typename List>
using enumerate = enumerate_impl<List, std::make_index_sequence<count<List>>>::type;

// flip_helper
template <typename Pair>
struct flip_helper_impl;

template <template <typename...> typename List, typename First, typename Second>
struct flip_helper_impl<List<First, Second>> {
  using type = List<Second, First>;
};

template <typename Pair>
using flip_helper = flip_helper_impl<Pair>::type;

// cut_mup
template <typename List>
struct cut_map_impl;

template <template <typename...> typename List, typename... Types>
struct cut_map_impl<List<Types...>> {
  using type = List<second<Types>...>;
};

template <typename List>
using cut_map = cut_map_impl<List>::type;

// contains<T, List>
template <typename T, typename List>
struct contains_impl;

template <typename T, template <typename...> typename List, typename... Types>
struct contains_impl<T, List<Types...>> {
  static constexpr bool value = (std::is_same_v<T, Types> || ...);
};

template <typename T, typename List>
inline constexpr bool contains = contains_impl<T, List>::value;

// flip<List>, List -- список пар
template <typename List>
struct flip_impl;

template <template <typename...> typename List, typename... Pairs>
struct flip_impl<List<Pairs...>> {
  using type = List<flip_helper<Pairs>...>;
};

template <typename List>
using flip = flip_impl<List>::type;

// index_of_unique<T, List>
template <typename T, typename List>
static constexpr std::size_t index_of_unique =
    std::integral_constant<std::size_t, second<map_find<T, flip<enumerate<List>>>>::value>::value;

// flatten<List>
template <typename List>
struct flatten_impl;

template <typename List>
using flatten = flatten_impl<List>::type;

template <template <typename...> typename List, typename... ToFlat>
struct flatten_impl<List<ToFlat...>> {
  using type = apply<List, concat_fast<typename flatten_impl<ToFlat>::type...>>;
};

template <typename Type>
struct flatten_impl {
  using type = type_list<Type>;
};

template <template <typename...> typename List>
struct flatten_impl<List<>> {
  using type = List<>;
};

// merge_sort<List, Compare>
template <typename List1, typename List2, template <typename...> typename Compare>
struct merge_impl {
  using type = conditional<
      Compare<second<front<List1>>, second<front<List2>>>::value,
      concat_fast<type_list<front<List1>>, typename merge_impl<pop_front<List1>, List2, Compare>::type>,
      concat_fast<type_list<front<List2>>, typename merge_impl<List1, pop_front<List2>, Compare>::type>>;
};

template <typename List1, template <typename...> typename List2, template <typename...> typename Compare>
struct merge_impl<List1, List2<>, Compare> {
  using type = List1;
};

template <template <typename...> typename List1, typename List2, template <typename...> typename Compare>
struct merge_impl<List1<>, List2, Compare> {
  using type = List2;
};

template <typename List, template <typename...> typename Compare>
struct merge_sort_impl {
  static constexpr std::size_t mid = (2 * front<front<List>>::value + count<List>) / 2;

  template <typename Pair>
  struct first_half {
    static constexpr bool value = front<Pair>::value < mid;
  };

  template <typename Pair>
  struct second_half {
    static constexpr bool value = !first_half<Pair>::value;
  };

  using left = filter<first_half, List>;
  using right = filter<second_half, List>;

  using sorted_left = typename merge_sort_impl<left, Compare>::type;
  using sorted_right = typename merge_sort_impl<right, Compare>::type;

  using type = typename merge_impl<sorted_left, sorted_right, Compare>::type;
};

template <template <typename...> typename List, typename First, template <typename...> typename Compare>
struct merge_sort_impl<List<First>, Compare> {
  using type = List<First>;
};

template <template <typename...> typename List, template <typename...> typename Compare>
struct merge_sort_impl<List<>, Compare> {
  using type = List<>;
};

template <typename List>
struct get_lst;

template <template <typename...> typename List, typename... Types>
struct get_lst<List<Types...>> {
  using type = List<Types...>;
};

template <typename List, template <typename...> typename Compare>
struct merge_sort_for_type;

template <template <typename...> typename List, typename... Types, template <typename...> typename Compare>
struct merge_sort_for_type<List<Types...>, Compare> {
  using type = apply<List, cut_map<typename merge_sort_impl<enumerate<List<Types...>>, Compare>::type>>;
};

template <typename List, template <typename...> typename Compare>
using merge_sort = merge_sort_for_type<List, Compare>::type;
} // namespace tl
