#pragma once

#include "App.hpp"

#include <concepts>
#include <utility>

#define NAMED_MANAGER_OBJECT(_type)                   \
private:                                              \
  friend class ManagerType;                           \
  explicit _type(QObject *owner = nullptr);           \
  virtual ~_type() {                                  \
      _type::deinitializeImpl();                      \
  }                                                   \
public:                                               \
  static _type &instance(QObject *parent = nullptr) { \
    return ManagerType::instance<_type>(parent);      \
  }                                                   \
  static bool initialize(QObject *parent = nullptr) { \
    return instance(parent).initializeImpl();         \
  }                                                   \
  static void deinitialize() {                        \
    instance().deinitializeImpl();                    \
  }                                                   \
private:                                              \
  bool initializeImpl() {                             \
    if (!m_initialized)                               \
      m_initialized = initializeInstance();           \
    return m_initialized;                             \
  }                                                   \
  void deinitializeImpl() {                           \
    if (!m_initialized)                               \
      return;                                         \
    deinitializeInstance();                           \
    m_initialized = false;                            \
  }                                                   \
  virtual bool initializeInstance() final override;   \
  virtual void deinitializeInstance() final override

#define MANAGER_OBJECT NAMED_MANAGER_OBJECT(Manager);

class ManagerType
{
protected:
  template<std::derived_from<ManagerType> T, class ... Args>
  static T &instance(Args &&... args)
  {
    static T g_manager { std::forward<Args>(args)... };

    return g_manager;
  }

  virtual QObject *owner() const { return m_owner ? m_owner : &App::instance(); }

protected:
  ManagerType(QObject *owner)  : m_owner{ owner }
  {}

protected:
  virtual bool initializeInstance() = 0;
  virtual void deinitializeInstance() = 0;

protected:
  QObject *m_owner = nullptr;
  bool    m_initialized = false;
};
