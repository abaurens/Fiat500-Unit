#pragma once

#include <optional>

template<class T>
class Local
{
public:
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_void_v<T>);

  constexpr Local() noexcept = default;
  constexpr Local(std::nullopt_t) noexcept : m_value{ std::nullopt }
  {}

  // Construct from value
  constexpr Local(const T &value) : m_value{ value }
  {}
  constexpr Local(T &&value) : m_value{ std::move(value) }
  {}

  // Construct from optional
  constexpr Local(const std::optional<T> &value) : m_value{ value }
  {}
  constexpr Local(std::optional<T> &&value) : m_value{ std::move(value) }
  {}

  // Data
  template<class... Args>
  constexpr T &emplace(Args&&... args) {
    return m_value.emplace(std::forward<Args>(args)...);
  }

  constexpr void reset() noexcept {
    m_value.reset();
  }

  constexpr T &value() {
    return m_value.value();
  }

  constexpr const T &value() const {
    return m_value.value();
  }

  [[nodiscard]]
  constexpr bool hasValue() const noexcept {
    return m_value.has_value();
  }

  constexpr explicit operator bool() const noexcept {
    return hasValue();
  }

  // Access operators
  constexpr T &operator*() & noexcept {
    return *m_value;
  }

  constexpr const T &operator*() const & noexcept {
    return *m_value;
  }

  constexpr T &&operator*() && noexcept {
    return *std::move(m_value);
  }

  constexpr T *operator->() noexcept {
    return &*m_value;
  }

  constexpr const T *operator->() const noexcept {
    return &*m_value;
  }

  // Compare operators
  friend constexpr bool operator==(const Local &lhs, const Local &rhs) {
    return lhs.m_value == rhs.m_value;
  }

  template<class U> requires (
    !std::same_as<std::remove_cvref_t<U>, Local> &&
    requires(const std::optional<T> &value, const U &other) { value == other; }
  )
  friend constexpr bool operator==(const Local &lhs, const U &rhs) {
    return lhs.m_value == rhs;
  }

  friend constexpr bool operator==(const Local &lhs, std::nullopt_t) noexcept {
    return !lhs.m_value;
  }

  friend constexpr auto operator<=>(const Local &lhs, const Local &rhs) requires std::three_way_comparable<T> {
    return lhs.m_value <=> rhs.m_value;
  }

  template<class U> requires (
    !std::same_as<std::remove_cvref_t<U>, Local> &&
    requires(const std::optional<T> &value, const U &other) { value <=> other; }
  )
  friend constexpr auto operator<=>(const Local &lhs, const U &rhs) {
    return lhs.m_value <=> rhs;
  }

  friend constexpr auto operator<=>(const Local &lhs, std::nullopt_t) noexcept {
    return lhs.m_value <=> std::nullopt;
  }

  // std::optional interoperability
  constexpr std::optional<T> &optional() noexcept {
    return m_value;
  }

  constexpr const std::optional<T> &optional() const noexcept {
    return m_value;
  }

  constexpr operator std::optional<T>() const & {
    return m_value;
  }

  constexpr operator std::optional<T>() && {
    return std::move(m_value);
  }

private:
  std::optional<T> m_value;
};

template<class T>
class Local<T&>
{
public:
  constexpr Local() noexcept = default;
  constexpr Local(std::nullopt_t) noexcept {}

  // Construct from reference
  constexpr Local(T &value) noexcept : m_value{ &value }
  {}

  constexpr Local(const Local &) noexcept = default;

  constexpr Local &operator=(const Local &) noexcept = default;

  // Data
  constexpr T &emplace(T &value) noexcept {
    m_value = &value;
    return value;
  }

  constexpr void reset() noexcept {
    m_value = nullptr;
  }

  constexpr T &value() {
    return *m_value;
  }

  constexpr const T &value() const {
    return *m_value;
  }

  [[nodiscard]]
  constexpr bool hasValue() const noexcept {
    return m_value != nullptr;
  }

  constexpr explicit operator bool() const noexcept {
    return hasValue();
  }

  // Access operators
  constexpr T &operator*() const noexcept {
    return *m_value;
  }

  constexpr T *operator->() const noexcept {
    return m_value;
  }

  // Compare operators
  friend constexpr bool operator==(const Local &lhs, const Local &rhs) {
    if (lhs && rhs)
      return *lhs == *rhs;

    return lhs.hasValue() == rhs.hasValue();
  }

  template<class U> requires (
    !std::same_as<std::remove_cvref_t<U>, Local> &&
    requires(const T &value, const U &other) { value == other; }
  )
  friend constexpr bool operator==(const Local &lhs, const U &rhs) {
    return lhs && *lhs == rhs;
  }

  friend constexpr bool operator==(const Local &lhs, std::nullopt_t) noexcept {
    return !lhs;
  }

  friend constexpr auto operator<=>(const Local &lhs, const Local &rhs) requires std::three_way_comparable<T> {
    if (lhs && rhs)
      return *lhs <=> *rhs;

    return lhs.hasValue() <=> rhs.hasValue();
  }

  //template<class U> requires (
  //  !std::same_as<std::remove_cvref_t<U>, Local> &&
  //  requires(const T &value, const U &other) { value <=> other; }
  //)
  //friend constexpr auto operator<=>(const Local &lhs, const U &rhs) {
  //  if (lhs)
  //    return *lhs <=> rhs;

  //  return std::strong_ordering::less;
  //}

  //friend constexpr auto operator<=>(const Local &lhs, std::nullopt_t) noexcept {
  //  return lhs.m_value <=> std::nullopt;
  //}

private:
  T *m_value = nullptr;
};

template<>
class Local<void>
{
public:
  constexpr Local() noexcept = default;
  constexpr Local(std::nullopt_t) noexcept {}

  constexpr Local(const Local &) noexcept = default;

  constexpr Local &operator=(const Local &) noexcept = default;

  // Data
  constexpr void emplace() noexcept {
    m_value = true;
    return;
  }

  constexpr void reset() noexcept {
    m_value = false;
  }

  [[nodiscard]]
  constexpr bool hasValue() const noexcept {
    return m_value;
  }

  constexpr explicit operator bool() const noexcept {
    return hasValue();
  }

  // Compare operators
  friend constexpr bool operator==(const Local &lhs, const Local &rhs) {
    return lhs.hasValue() == rhs.hasValue();
  }

  template<class U> requires (!std::same_as<std::remove_cvref_t<U>, Local>)
  friend constexpr bool operator==(const Local &lhs, const U &rhs) {
    return false;
  }

  friend constexpr bool operator==(const Local &lhs, std::nullopt_t) noexcept {
    return !lhs;
  }

  friend constexpr auto operator<=>(const Local &lhs, const Local &rhs) {
    return lhs.hasValue() <=> rhs.hasValue();
  }

private:
  bool m_value = false;
};
