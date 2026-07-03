#include "AdapterPanel.hpp"

#include "sizes.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpressionValidator>

AdapterPanel::AdapterPanel(Adapter &adapter, QWidget *parent)
  : QGroupBox{ tr("Adapter"), parent }, m_adapter{ &adapter }
{
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  QHBoxLayout *aliasLayout = new QHBoxLayout();
  {
    m_alias = new QLineEdit();
    m_setAlias = new QPushButton(tr("Set Alias"));
    aliasLayout->addWidget(m_alias);
    aliasLayout->addWidget(m_setAlias);
  }

  m_powered = new QCheckBox(tr("Powered"));
  m_discoverable = new QCheckBox(tr("Discoverable"));
  m_discovering = new QCheckBox(tr("Discovering"));

  QHBoxLayout *discoveryLayout = new QHBoxLayout();
  {
    m_startDiscovery = new QPushButton(tr("Start discovery"));
    m_stopDiscovery = new QPushButton(tr("Stop discovery"));
    discoveryLayout->addWidget(m_startDiscovery);
    discoveryLayout->addWidget(m_stopDiscovery);
  }

  mainLayout->addLayout(aliasLayout);
  mainLayout->addWidget(m_powered);
  mainLayout->addWidget(m_discoverable);
  mainLayout->addWidget(m_discovering);
  mainLayout->addLayout(discoveryLayout);

  QFont fnt = font();
  fnt.setPointSize(UI::Size::font);
  setFont(fnt);
  // QGuiApplication::setFont()

  mainLayout->setSpacing(UI::Size::margin);
  mainLayout->setContentsMargins(UI::Size::margins);

  aliasLayout->setSpacing(UI::Size::spacing);
  discoveryLayout->setSpacing(UI::Size::spacing);

  APPLY_MIN_SIZE(m_alias);
  APPLY_MIN_SIZE(m_powered);
  APPLY_MIN_SIZE(m_discoverable);
  APPLY_MIN_SIZE(m_discovering);
  APPLY_MIN_SIZE(m_setAlias);
  APPLY_MIN_SIZE(m_startDiscovery);
  APPLY_MIN_SIZE(m_stopDiscovery);

  m_setAlias->setFixedWidth(110);
  m_startDiscovery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  m_stopDiscovery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  /// Alias
  {
    m_rxAlias = QRegularExpression("(?:[A-Za-z _-][A-Za-z0-9 _-]*)?");

    const QString &alias = adapter.alias();
    const QRegularExpressionValidator *validator = new QRegularExpressionValidator(m_rxAlias, this);
    m_alias->setValidator(validator);

    m_alias->setText(alias);
    m_alias->setPlaceholderText(alias);
    m_setAlias->setEnabled(false);

    connect(
      m_adapter, &Adapter::aliasChanged,
      [this](const QString &alias)
      {
        m_alias->setText(alias);
        m_alias->setPlaceholderText(alias);
        m_setAlias->setEnabled(false);
      }
    );

    connect(
      m_alias, &QLineEdit::textChanged,
      [this](const QString &text)
      {
        const QString &currentAlias = m_adapter->alias();
        const QString &candidateAlias = text.isEmpty() ? m_alias->placeholderText() : text;

        m_setAlias->setEnabled(currentAlias != candidateAlias);
      }
    );

    connect(
      m_setAlias, &QPushButton::clicked,
      [this]()
      {
        const QString &alias = m_alias->text().isEmpty() ? m_alias->placeholderText() : m_alias->text();
        m_adapter->setAlias(alias);
      }
    );
  }

  /// Powered
  {
    const bool powered = adapter.powered();

    m_powered->setChecked(powered);

    connect(
      m_adapter, &Adapter::poweredChanged,
      [this](bool powered) { m_powered->setChecked(powered); }
    );

    connect(
      m_powered, &QCheckBox::clicked,
      m_adapter, &Adapter::setPowered
    );
  }

  /// Discoverable
  {
    const bool discoverable = adapter.discoverable();

    m_discoverable->setChecked(discoverable);

    connect(
      m_adapter, &Adapter::discoverableChanged,
      [this](bool discoverable) { m_discoverable->setChecked(discoverable); }
    );

    connect(
      m_discoverable, &QCheckBox::clicked,
      m_adapter,      &Adapter::setDiscoverable
    );
  }

  /// Discovery
  {
    m_discovering->setEnabled(false);

    const bool discovering = adapter.discovering();
    m_discovering->setChecked(discovering);
    m_startDiscovery->setEnabled(!discovering);
    m_stopDiscovery->setEnabled(discovering);

    connect(
      m_adapter, &Adapter::discoveringChanged,
      [this](bool discovering)
      {
        m_discovering->setChecked(discovering);
        m_startDiscovery->setEnabled(!discovering);
        m_stopDiscovery->setEnabled(discovering);
      }
    );

    connect(
      m_startDiscovery, &QPushButton::clicked,
      m_adapter,        &Adapter::startDiscovery
    );

    connect(
      m_stopDiscovery, &QPushButton::clicked,
      m_adapter,       &Adapter::stopDiscovery
    );
  }
}
