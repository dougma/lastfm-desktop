#ifndef SOURCE_SELECTOR_WIDGET_H
#define SOURCE_SELECTOR_WIDGET_H

#include <QWidget>

class QLineEdit;
class QListWidget;
class QPushButton;
class QListWidgetItem;

class SourceSelectorWidget : public QWidget
{
    Q_OBJECT;

public:
    SourceSelectorWidget(QLineEdit* edit, QWidget* parent = 0);
    QListWidget* list();

signals:
    void add(const QString& item);

protected:
    QLineEdit* m_edit;
    QPushButton* m_button;
    QListWidget* m_list;

private slots:
    void emitAdd();
    void onListItemActivated(QListWidgetItem* item);
};

#endif
