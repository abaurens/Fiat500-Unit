#pragma once

#include <string>

/// Char trait for case insensitive comparison
template<class CharT>
struct ci_char_traits : public std::char_traits<CharT>
{
  static constexpr CharT to_lower(CharT ch) noexcept
  {
    constexpr CharT offset = ('a' - 'A');

    if (ch >= 'A' && ch <= 'Z')
      return ch + offset;
    return ch;
  }

  static constexpr bool eq(CharT c1, CharT c2) noexcept
  {
    return to_lower(c1) == to_lower(c2);
  }

  static constexpr bool lt(CharT c1, CharT c2)
  {
    return to_lower(c1) < to_lower(c2);
  }

  static constexpr int compare(const CharT* left, const CharT* right, std::size_t length) noexcept
  {
    CharT lc;
    CharT rc;
    while (length-- != 0)
    {
      lc = to_lower(*left);
      rc = to_lower(*right);

      if (lc < rc) return -1;
      if (lc > rc) return 1;

      ++left;
      ++right;
    }
    return 0;
  }

  static const CharT* find(const CharT *str, std::size_t length, const CharT &c)
  {
    const auto lc{ to_lower(c)};

    while (length-- != 0)
    {
      if (to_lower(*str) == lc)
        return str;
      str++;
    }
    return nullptr;
  }
};

template<template<class C> class DstTraits, class CharT, template<class C> class SrcTraits>
constexpr std::basic_string_view<CharT, DstTraits<CharT>> traits_cast(const std::basic_string_view<CharT, SrcTraits<CharT>> src) noexcept
{
  return { src.data(), src.size() };
}
