#pragma once

#include <QMainWindow>

#include <QLineEdit>
#include <QPushButton>


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

signals:
  void nameChanged(const QString &);
  void discClicked();

private slots:
  void onEditClick();
  void onDiscClick();

private:
  QLineEdit   *edit;
  QPushButton *btn;
  QPushButton *disc;
};
