#include "MainWindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *editLayout = new QHBoxLayout();

  setCentralWidget(centralWidget);
  centralWidget->setLayout(mainLayout);

  edit = new QLineEdit("BT Name");
  btn = new QPushButton("Set");
  disc = new QPushButton("Discoverable");

  mainLayout->addLayout(editLayout);
  editLayout->addWidget(edit);
  editLayout->addWidget(btn);

  mainLayout->addWidget(disc);

  connect(
    btn,  &QPushButton::clicked,
    this, &MainWindow::onEditClick
  );

  connect(
    disc,  &QPushButton::clicked,
    this,  &MainWindow::onDiscClick
  );
}

MainWindow::~MainWindow() = default;

void MainWindow::onEditClick()
{
  if (!edit->text().isEmpty())
    emit nameChanged(edit->text());
}

void MainWindow::onDiscClick()
{
  emit discClicked();
}

