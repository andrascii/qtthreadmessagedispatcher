#pragma once

#define DEFINE_METHOD_CHECK(Method) \
namespace MetaHelpers { \
template <typename ObjectType, typename... ParamTypes> \
YesType& has_method_helper_##Method(decltype(std::declval<ObjectType&>().Method(std::declval<ParamTypes>()...))* p); \
\
template <typename...> \
NoType& has_method_helper_##Method(...); \
\
template <typename ObjectType, typename... ParamTypes> \
struct HasMethod_##Method \
	: std::integral_constant<bool, sizeof(has_method_helper_##Method<ObjectType, ParamTypes...>(nullptr)) == sizeof(YesType)> \
{}; \
}  // namespace MetaHelpers

namespace MetaHelpers {

using YesType = char;
using NoType = char[2];

template <template <typename> class>
constexpr bool is_all_of() {
  return true;
}

template <template <typename> class Pred, typename Head, typename... Tail>
constexpr bool is_all_of() {
  return Pred<Head>::value && is_all_of<Pred, Tail...>();
}

// Calls for each argument from argument packet passed functor.
template <typename F, typename Head>
void call_for_all_arguments_in_pack(F f, Head&& head) {
  f(head);
}

template <typename F, typename Head, typename... Tail>
void call_for_all_arguments_in_pack(F f, Head&& head, Tail&&... tail) {
  f(head);
  call_for_all_arguments_in_pack(f, std::forward<Tail>(tail)...);
}

// Calls for each argument from argument packet passed functor.
// Returns true if for all passed parameters functor returns true.
template <typename F, typename Head>
bool is_each_argument(F f, Head&& head) {
  return f(head);
}

template <typename F, typename Head, typename... Tail>
bool is_each_argument(F f, Head&& head, Tail&&... tail) {
  return f(head) && is_each_argument(f, std::forward<Tail>(tail)...);
}

}  // namespace MetaHelpers
