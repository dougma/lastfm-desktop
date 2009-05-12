#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <QWidget>
#include <QStack>

class QPushButton;
class QHBoxLayout;

class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    HistoryWidget(QWidget* parent = 0);
    ~HistoryWidget();

    bool pop();

signals:
    void clicked(int position, const QString& text);

public slots:
    void newItem(const QString& text);

private slots:
    void onItemClicked();

private:
    QHBoxLayout* m_layout;
    QStack<QPushButton*> m_labels;
};

#endif
