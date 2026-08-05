#pragma once

#include "CaseInsensitiveCharTrait.hpp"

#include <type_traits>
#include <concepts>

#include <QString>
#include <QStringLiteral>

#include <frozen/map.h>
#include <frozen/string.h>
#include <frozen/unordered_map.h>

namespace std
{
  template<typename T>
  concept enumeration = std::is_enum_v<T>;
}

namespace Enums
{
  template<class CharT, template<class C> class CharTraits>
  using basic_name = std::basic_string_view<CharT, CharTraits<CharT>>;

  template<class Nm, template<class C> class CharTraits>
  concept NameOrQName = std::same_as<basic_name<char, CharTraits>, Nm> || std::same_as<basic_name<c16, CharTraits>, Nm>;

  template<class Enum, std::enumeration T, template<class C> class CharTraits>
  struct Value
  {
  private:
    using Name = basic_name<char, CharTraits>;
    using QName = basic_name<c16, CharTraits>;

  public:
    constexpr Value(T v, Name n, QName qn)
      : m_val{v}, m_name{n}, m_qname{qn}
    {}

    static constexpr size_t hash(Value v, size_t seed) {
      return v.hash(seed);
    }

    constexpr Name name() const { return m_name; }
    constexpr QName qname() const { return m_qname; }

    template<std::integral Ret>
    constexpr Ret valueAs() const { return static_cast<Ret>(m_val); }

    // String conversion overloads
    template<std::same_as<Name> N>
    constexpr N valueAs() const { return name(); }
    template<std::same_as<QName> N>
    constexpr N valueAs() const { return qname(); }
    template<std::same_as<const char *> N>
    constexpr N valueAs() const { return name().data(); }
    template<std::same_as<std::string> N>
    constexpr N valueAs() const { return std::string{ name() }; }
    template<std::same_as<QString> N>
    constexpr N valueAs() const { return Qt::StringLiterals::operator""_s(qname().data(), qname().size()); }

    constexpr size_t value() const { return static_cast<size_t>(m_val); }

    template<std::integral Ret>
    constexpr explicit operator Ret() const { return static_cast<Ret>(m_val); }

    constexpr operator T() const { return m_val; }

    constexpr auto operator<=>(Value other) const {
      return m_val <=> other.m_val;
    }
    constexpr bool operator==(Value other) const {
      return m_val == other.m_val;
    }
    constexpr size_t hash(size_t seed) const {
      return seed ^ static_cast<size_t>(m_val);
    }

  private:
    friend Enum;
    T     m_val;
    Name  m_name;
    QName m_qname;
  };

  template<class... Ts>
  constexpr size_t ArgCount(Ts &&... args) {
    return sizeof...(Ts);
  }

  template<template<class C> class CharTraits, NameOrQName<CharTraits> Nm, std::enumeration T, size_t Len>
  using FromNameMap = frozen::map<Nm, T, Len>;
}

#define _enm_PARENS ()

// Rescan macro tokens 256 times
#define _enm_EXPAND(...)  _enm_EXPAND1(_enm_EXPAND1(_enm_EXPAND1(_enm_EXPAND1(__VA_ARGS__))))
#define _enm_EXPAND1(...) _enm_EXPAND2(_enm_EXPAND2(_enm_EXPAND2(_enm_EXPAND2(__VA_ARGS__))))
#define _enm_EXPAND2(...) _enm_EXPAND3(_enm_EXPAND3(_enm_EXPAND3(_enm_EXPAND3(__VA_ARGS__))))
#define _enm_EXPAND3(...) _enm_EXPAND4(_enm_EXPAND4(_enm_EXPAND4(_enm_EXPAND4(__VA_ARGS__))))
#define _enm_EXPAND4(...) __VA_ARGS__

#define _enm_FOREACH(macro, ...) __VA_OPT__(_enm_EXPAND(_enm_FOREACH_HELPER(macro, __VA_ARGS__)) )
#define _enm_FOREACH_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(_enm_FOREACH_AGAIN _enm_PARENS (macro, __VA_ARGS__) )
#define _enm_FOREACH_AGAIN() _enm_FOREACH_HELPER

#define _enm_STAT_CEXP inline static constexpr
#define _enm_NAME(name) #name,
#define _enm_QNAME(name) u ## #name,
#define _enm_VALUE(name) Values::name,
#define _enm_NAME_TO_ENUM(name) { #name, Values::name },
#define _enm_QNAME_TO_ENUM(name) { u ## #name, Values::name },
#define _enm_CONSTRUCT(name) { Values::name, #name, u ## #name }
#define _enm_DECLARE(name) _enm_STAT_CEXP Value name { Values::name, #name, u ## #name };

#define MAKE_ENUM_CT(_name, _char_traits, ...)                                                \
struct _name                                                                                  \
{                                                                                             \
private:                                                                                      \
  using Self = _name;                                                                         \
  enum class Values : size_t { __VA_ARGS__, Unknown = std::numeric_limits<size_t>::max() };   \
  _enm_STAT_CEXP size_t Count = Enums::ArgCount(_enm_FOREACH(_enm_VALUE, __VA_ARGS__) 0) - 1; \
  using Value = Enums::Value<_name, Values, _char_traits>;                                    \
  using Name = typename Value::Name;                                                          \
  using QName = typename Value::QName;                                                        \
  using Names = std::array<Name, Count>;                                                      \
  using QNames = std::array<QName, Count>;                                                    \
  using Parser = Enums::FromNameMap<_char_traits, Name, Values, Count>;                       \
  using QParser = Enums::FromNameMap<_char_traits, QName, Values, Count>;                     \
  _enm_STAT_CEXP Names s_names = { _enm_FOREACH(_enm_NAME, __VA_ARGS__) };                    \
  _enm_STAT_CEXP QNames s_qnames = { _enm_FOREACH(_enm_QNAME, __VA_ARGS__) };                 \
  _enm_STAT_CEXP Parser s_parser = { _enm_FOREACH(_enm_NAME_TO_ENUM, __VA_ARGS__) };          \
  _enm_STAT_CEXP QParser s_qparser = { _enm_FOREACH(_enm_QNAME_TO_ENUM, __VA_ARGS__) };       \
  _enm_STAT_CEXP _name FromUnderlying(const Values val) {                                     \
    if (val == Values::Unknown)                                                               \
      return _name::Unknown;                                                                  \
    const size_t id = static_cast<size_t>(val);                                               \
    return Value{ val, s_names[id], s_qnames[id] };                                           \
  }                                                                                           \
public:                                                                                       \
  using Underlying = Values;                                                                  \
  _enm_DECLARE(Unknown)                                                                       \
  _enm_STAT_CEXP size_t count() { return Count; }                                             \
  _enm_STAT_CEXP Names names() { return s_names; }                                            \
  _enm_STAT_CEXP _name FromName(const Enums::basic_name<char, std::char_traits> name) {       \
    if (s_parser.contains(traits_cast<_char_traits>(name)))                                   \
      return FromUnderlying(_name::s_parser.at(traits_cast<_char_traits>(name)));             \
    return _name::Unknown;                                                                    \
  }                                                                                           \
  _enm_STAT_CEXP _name FromName(const Enums::basic_name<c16, std::char_traits> name) {        \
    if (s_qparser.contains(traits_cast<_char_traits>(name)))                                  \
      return FromUnderlying(_name::s_qparser.at(traits_cast<_char_traits>(name)));            \
    return _name::Unknown;                                                                    \
  }                                                                                           \
  _enm_STAT_CEXP size_t hash(_name value, size_t seed) { return hash(value, seed); }          \
  _enm_STAT_CEXP size_t hash(Value value, size_t seed) { return Value::hash(value, seed); }   \
  constexpr _name(Value v) : m_data{ v } {}                                                   \
  constexpr operator Value() const { return m_data; }                                         \
  template<std::integral T>                                                                   \
  constexpr explicit operator T() const { return static_cast<T>(m_data); }                    \
  constexpr operator Values() const { return static_cast<Values>(m_data); }                   \
  constexpr Name name() const { return m_data.name(); }                                       \
  template<class Ret>                                                                         \
  constexpr Ret valueAs() const { return m_data.valueAs<Ret>(); }                             \
  constexpr size_t value() const { return m_data.value(); }                                   \
  constexpr size_t hash(size_t seed) const { return Value::hash(m_data, seed); }              \
  template<std::integral T>                                                                   \
  constexpr auto operator<=>(T ov) const { return m_data.m_val <=> ov; }                      \
  template<std::integral T>                                                                   \
  constexpr bool operator==(T ov) const { return m_data.m_val == ov; }                        \
  constexpr auto operator<=>(Value other) const { return m_data <=> other; }                  \
  constexpr bool operator==(Value other) const { return m_data == other; }                    \
  constexpr auto operator<=>(Self other) const { return m_data <=> other.m_data; }            \
  constexpr bool operator==(Self other) const { return m_data == other.m_data; }              \
  template<class OS>                                                                          \
  friend OS &operator<<(OS &&os, const _name &value) { return (os << value.m_data); }         \
public:                                                                                       \
  _enm_FOREACH(_enm_DECLARE, __VA_ARGS__)                                                     \
private:                                                                                      \
  Value m_data;                                                                               \
}

#define MAKE_ENUM_CI(_name, ...) MAKE_ENUM_CT(_name, ci_char_traits, __VA_ARGS__)
#define MAKE_ENUM(_name, ...) MAKE_ENUM_CT(_name, std::char_traits, __VA_ARGS__)
