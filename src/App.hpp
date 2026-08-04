#pragma once

#include "MainWindow.hpp"

#include <QApplication>

class App : public QApplication
{
public:
  App(int &ac, char **av, int flags = ApplicationFlags);
  ~App();

  int run();

  static App &instance() { return *static_cast<App*>(QCoreApplication::instance()); }
  static MainWindow &window() { return *instance().m_window; }

private:
  void parseArgs();

  void setupDBus();
  void setupPipewire();

  void loadTranslations();

private:
  MainWindow *m_window = nullptr;
};
