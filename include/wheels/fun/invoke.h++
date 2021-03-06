// Wheels - various C++ utilities
//
// Written in 2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// INVOKE protocol

#ifndef WHEELS_FUN_INVOKE_HPP
#define WHEELS_FUN_INVOKE_HPP

#include <wheels/meta/not.h++>
#include <wheels/meta/any.h++>
#include <wheels/meta/unqual.h++>
#include <wheels/meta/enable_if.h++>
#include <wheels/meta/class_of.h++>
#include <wheels/meta/is_deduced.h++>

#include <wheels/meta/bool.h++>
#include <wheels/meta/void.h++>

#include <wheels/meta/depend_on.h++>

#include <type_traits> // is_member_function_pointer, is_base_of, is_member_object_pointer, is_convertible, is_void
#include <utility> // declval, forward

namespace wheels {
    namespace fun {
        namespace detail {
            template <typename Fun, typename Obj, typename... Args,
                      meta::EnableIf<std::is_member_function_pointer<meta::Unqual<Fun>>,
                                     std::is_base_of<meta::ClassOf<meta::Unqual<Fun>>, meta::Unqual<Obj>>>...,
                      typename Result = decltype((std::declval<Obj>().*std::declval<Fun>())(std::declval<Args>()...)),
                      bool NoExcept = noexcept((std::declval<Obj>().*std::declval<Fun>())(std::declval<Args>()...))>
            Result invoke(Fun&& fun, Obj&& obj, Args&&... args) noexcept(NoExcept) {
                return (std::forward<Obj>(obj).*std::forward<Fun>(fun))(std::forward<Args>(args)...);
            }
            template <typename Fun, typename Obj, typename... Args,
                      meta::EnableIf<std::is_member_function_pointer<meta::Unqual<Fun>>,
                                     meta::Not<std::is_base_of<meta::ClassOf<meta::Unqual<Fun>>, meta::Unqual<Obj>>>>...,
                      typename Result = decltype(((*std::declval<Obj>()).*std::declval<Fun>())(std::declval<Args>()...)),
                      bool NoExcept = noexcept(((*std::declval<Obj>()).*std::declval<Fun>())(std::declval<Args>()...))>
            Result invoke(Fun&& fun, Obj&& obj, Args&&... args) noexcept(NoExcept) {
                return ((*std::forward<Obj>(obj)).*std::forward<Fun>(fun))(std::forward<Args>(args)...);
            }
            template <typename Fun, typename Obj,
                      meta::EnableIf<std::is_member_object_pointer<meta::Unqual<Fun>>,
                                     std::is_base_of<meta::ClassOf<meta::Unqual<Fun>>, meta::Unqual<Obj>>>...,
                      typename Result = decltype(std::declval<Obj>().*std::declval<Fun>()),
                      bool NoExcept = noexcept(std::declval<Obj>().*std::declval<Fun>())>
            Result invoke(Fun&& fun, Obj&& obj) noexcept(NoExcept) {
                return std::forward<Obj>(obj).*std::forward<Fun>(fun);
            }
            template <typename Fun, typename Obj,
                      meta::EnableIf<std::is_member_object_pointer<meta::Unqual<Fun>>,
                                     meta::Not<std::is_base_of<meta::ClassOf<meta::Unqual<Fun>>, meta::Unqual<Obj>>>>...,
                      typename Result = decltype((*std::declval<Obj>()).*std::declval<Fun>()),
                      bool NoExcept = noexcept((*std::declval<Obj>()).*std::declval<Fun>())>
            Result invoke(Fun&& fun, Obj&& obj) noexcept(NoExcept) {
                return (*std::forward<Obj>(obj)).*std::forward<Fun>(fun);
            }
            template <typename Fun, typename... Args,
                      meta::DisableIf<std::is_member_pointer<meta::Unqual<Fun>>>...,
                      typename Result = decltype(std::declval<Fun>()(std::declval<Args>()...)),
                      bool NoExcept = noexcept(std::declval<Fun>()(std::declval<Args>()...))>
            Result invoke(Fun&& fun, Args&&... args) noexcept(NoExcept) {
                return std::forward<Fun>(fun)(std::forward<Args>(args)...);
            }
        } // namespace detail

        template <typename Explicit = meta::deduced, typename... T,
                  typename Computed = decltype(detail::invoke(std::declval<T>()...)),
                  typename Result = meta::If<meta::is_deduced<Explicit>, Computed, Explicit>,
                  meta::EnableIf<meta::Any<std::is_convertible<Computed, Result>, std::is_void<Result>>>...,
                  bool NoExcept = noexcept(detail::invoke(std::declval<T>()...))>
        Result invoke(T&&... t) noexcept(NoExcept) {
            return detail::invoke(std::forward<T>(t)...);
        }
    } // namespace fun
} // namespace wheels

#endif // WHEELS_FUN_INVOKE_HPP

