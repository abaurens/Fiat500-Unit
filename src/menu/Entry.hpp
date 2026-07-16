#pragma once

#include "menu/Menu.hpp"

#include <QGroupBox>

class Menu::Entry : public QWidget
{
  Q_OBJECT

public:
  qsizetype depth() const { return m_depth; }

  bool isRoot() const { return m_parent == nullptr; }

  QString &name() { return m_name; }
  const QString &name() const { return m_name; }

  Page *parent() { return m_parent; }
  const Page *parent() const { return m_parent; }

  Page *root();
  const Page *root() const;

protected:
  Entry(const QString &name, Page *parent = nullptr);
  virtual ~Entry() = default;

private:
  template<std::derived_from<Entry> T>
  bool isOfType() const
  {
    return dynamic_cast<const T*>(this) != nullptr;
  }

protected:
  Page *m_parent;
  QString m_name;
  qsizetype m_depth;
};

