#pragma once

#include <utility>

#define REMAP_RVALUES_FOR(MEMBER_FUNC_NAME)                                    \
  template <typename... ArgTypes>                                              \
  auto &&MEMBER_FUNC_NAME(ArgTypes&&... args) && {                             \
    return std::move(                                                          \
        static_cast<std::remove_pointer_t<decltype(this)> &>(*this)            \
            .MEMBER_FUNC_NAME(std::forward<ArgTypes>(args)...));                         \
  }
