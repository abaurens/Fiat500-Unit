#include "Breadcrumbs.hpp"
#include "menu/Page.hpp"

// using Page = Menu::Page;

Breadcrumbs::Breadcrumbs(QWidget *parent)
  : QWidget{ parent }, m_target{ nullptr }, m_layout{ new QHBoxLayout(this) }
{
  setContentsMargins(0, 0, 0, 0);

  m_layout->setSpacing(0);
  m_layout->addStretch();
}

void Breadcrumbs::setPath(const Menu::Page &path)
{
  qsizetype targetSize = path.depth();

  // Expand the button list if necessary
  ensureButtonCount(targetSize);

  // Remove or add buttons to the layout
  adjustLayoutTo(targetSize);

  // Update buttons text
  const Menu::Page *cur = &path;
  for (qsizetype i = targetSize; i > 0;)
  {
    QPushButton *button = m_buttons[--i];

    Q_ASSERT(cur != nullptr);
    Q_ASSERT(button != nullptr);

    m_pages[i] = cur;
    button->setText(cur->name());
    cur = cur->parent();
  }

  m_target = &path;
}

void Breadcrumbs::requestPage(qsizetype buttonId)
{
  const Menu::Page *page = m_pages.value(buttonId, nullptr);

  if (page)
    emit pageRequested(const_cast<Menu::Page&>(*page));
}

void Breadcrumbs::adjustLayoutTo(qsizetype depth)
{
  const qsizetype targetLayoutSize = depth + 1;

  if (m_layout->count() == targetLayoutSize)
    return;

  if (m_layout->count() > targetLayoutSize)
    return trimLayoutTo(depth);
  else
    return expandLayoutTo(depth);

  Q_ASSERT(m_layout->count() == targetLayoutSize);
}

void Breadcrumbs::trimLayoutTo(qsizetype count)
{
  qsizetype i = m_layout->count() - 1;

  // Remove excess buttons from the layout
  while (i-- > count)
  {
    auto *item = m_layout->takeAt(i);
    Q_ASSERT(item != nullptr);

    QWidget *widget = item->widget();
    Q_ASSERT(widget == m_buttons[i]);

    widget->setParent(nullptr);
  }

  // Sanity check the remaining buttons
  while (i-- > 0)
  {
    [[maybe_unused]] auto *item = m_layout->itemAt(i);
    Q_ASSERT(item != nullptr);
    Q_ASSERT(item->widget() == m_buttons[i]);
  }
}

void Breadcrumbs::expandLayoutTo(qsizetype count)
{
  const qsizetype currentSize = m_layout->count() - 1;

  qsizetype i;

  // Sanity check already present buttons
  for (i = 0; i < currentSize; ++i)
  {
    [[maybe_unused]] auto *item = m_layout->itemAt(i);
    Q_ASSERT(item != nullptr);
    Q_ASSERT(item->widget() == m_buttons[i]);
  }

  for (; i < count; ++i)
  {
    m_layout->insertWidget(i, m_buttons[i]);
  }
}

void Breadcrumbs::ensureButtonCount(qsizetype count)
{
  qsizetype size = m_buttons.size();

  if (size >= count)
    return;

  m_buttons.resize(count);

  while (size < count)
  {
    Button *button = new Button(size);
    connect(button, &Button::clicked, this, &Breadcrumbs::requestPage);

    m_buttons[size++] = button;
  }
}

Breadcrumbs::Button::Button(qsizetype id) : m_id(id)
{
  setMinimumHeight(50);
  setContentsMargins(0, 0, 0, 0);

  connect(this, &QPushButton::clicked, [this]{ clicked(m_id); });
}
