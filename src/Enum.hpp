#pragma once

#include <type_traits>
#include <concepts>
#include <array>

#include <frozen/map.h>
#include <frozen/string.h>

namespace std
{
  template<typename T>
  concept enumeration = std::is_enum_v<T>;
}

//#define MAKE_ENUM(_name, ...)                    \
//  enum class _name : size_t {                    \
//    __VA_ARGS__,                                 \
//    Count,                                       \
//    Unknown = std::numeric_limits<size_t>::max() \
//  }


namespace Enums
{
  using Name = std::string_view;

  template<class Enum, class T>
  struct Value
  {
  public:
    constexpr Value(T v, Name n)
      : m_val{v}, m_name{n}
    {}

    constexpr Name name() const { return m_name; }

    template<std::integral Ret>
    constexpr Ret valueAs() const { return static_cast<Ret>(m_val); }

    template<std::integral Ret>
    constexpr explicit operator Ret() const { return static_cast<Ret>(m_val); }

    constexpr auto operator<=>(Value other) const {
      return m_val <=> other.m_val;
    }
    constexpr bool operator==(Value other) const {
      return m_val == other.m_val;
    }

  private:
    friend Enum;
    T    m_val;
    Name m_name;
  };

  template<class... Ts>
  constexpr size_t ArgCount(Ts &&... args)
  {
    return sizeof...(Ts);
  }

  template<std::enumeration T, size_t Len>
  using FromNameMap = frozen::map<Name, T, Len>;
}

#define _enm_PARENS ()

// Rescan macro tokens 256 times
#define _enm_EXPAND(...)  _enm_EXPAND1(_enm_EXPAND1(_enm_EXPAND1(_enm_EXPAND1(__VA_ARGS__))))
#define _enm_EXPAND1(...) _enm_EXPAND2(_enm_EXPAND2(_enm_EXPAND2(_enm_EXPAND2(__VA_ARGS__))))
#define _enm_EXPAND2(...) _enm_EXPAND3(_enm_EXPAND3(_enm_EXPAND3(_enm_EXPAND3(__VA_ARGS__))))
#define _enm_EXPAND3(...) _enm_EXPAND4(_enm_EXPAND4(_enm_EXPAND4(_enm_EXPAND4(__VA_ARGS__))))
#define _enm_EXPAND4(...) __VA_ARGS__

#define _enm_FOR_EACH_ARGS(macro, ...) __VA_OPT__(_enm_EXPAND(_enm_FOR_EACH_ARGS_HELPER(macro, __VA_ARGS__)))
#define _enm_FOR_EACH_ARGS_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(_enm_FOR_EACH_ARGS_AGAIN _enm_PARENS (macro, __VA_ARGS__))
#define _enm_FOR_EACH_ARGS_AGAIN() _enm_FOR_EACH_ARGS_HELPER

#define _enm_ENUM_NAME(name) #name,
#define _enm_ENUM_VALUE(name) Values::name,
#define _enm_NAME_TO_ENUM(name) { #name, Values::name },
#define _enm_ENUM_DECLARE(name) inline static constexpr Value name{ Values::name, #name };

#define MAKE_ENUM(_name, ...)                           \
struct _name                                            \
{                                                       \
private:                                                \
  using Self = _name;                                   \
  using Name = std::string_view;                        \
  enum class Values : size_t                            \
  {                                                     \
    __VA_ARGS__,                                        \
    Unknown = std::numeric_limits<size_t>::max()        \
  };                                                    \
  static inline constexpr size_t Count =                \
  Enums::ArgCount(                                      \
    _enm_FOR_EACH_ARGS(_enm_ENUM_VALUE, __VA_ARGS__) 0  \
  ) - 1;                                                \
  using Value = Enums::Value<_name, Values>;            \
  using Names = std::array<Name, Count>;                \
  using Parser = Enums::FromNameMap<Values, Count>;     \
  static inline constexpr Names s_names = {             \
    _enm_FOR_EACH_ARGS(_enm_ENUM_NAME, __VA_ARGS__)     \
  };                                                    \
  static inline constexpr Parser s_parser = {           \
    _enm_FOR_EACH_ARGS(_enm_NAME_TO_ENUM, __VA_ARGS__)  \
  };                                                    \
public:                                                 \
  _enm_ENUM_DECLARE(Unknown)                            \
  static constexpr size_t count() { return Count; }     \
  static constexpr Names names() { return s_names; }    \
  static constexpr _name FromName(const Name name) {    \
    if (s_parser.contains(name))                        \
      return Value{ Type::s_parser.at(name), name };    \
    return _name::Unknown;                              \
  }                                                     \
  constexpr _name(Value v) : m_data{ v } {}             \
  constexpr operator Value() const { return m_data; }   \
  template<std::integral T>                             \
  constexpr explicit operator T() const {               \
    return static_cast<T>(m_data);                      \
  }                                                     \
  constexpr Name name() const { return m_data.name(); } \
  template<std::integral Ret>                           \
  constexpr Ret valueAs() const {                       \
    return m_data.valueAs<Ret>();                       \
  }                                                     \
  template<std::integral T>                             \
  constexpr auto operator<=>(T ov) const {              \
    return m_data.m_val <=> ov;                         \
  }                                                     \
  template<std::integral T>                             \
  constexpr bool operator==(T ov) const {               \
    return m_data.m_val == ov;                          \
  }                                                     \
  constexpr auto operator<=>(Value other) const {       \
    return m_data <=> other;                            \
  }                                                     \
  constexpr bool operator==(Value other) const {        \
    return m_data == other;                             \
  }                                                     \
  constexpr auto operator<=>(Self other) const {        \
    return m_data <=> other.m_data;                     \
  }                                                     \
  constexpr bool operator==(Self other) const {         \
    return m_data == other.m_data;                      \
  }                                                     \
public:                                                 \
  _enm_FOR_EACH_ARGS(_enm_ENUM_DECLARE, __VA_ARGS__)    \
private:                                                \
  Value m_data;                                         \
}
