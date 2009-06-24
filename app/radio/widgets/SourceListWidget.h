#ifndef SOURCE_LIST_WIDGET_H
#define SOURCE_LIST_WIDGET_H

#include <QWidget>

class QVBoxLayout;

class SourceListWidget : public QWidget
{
    Q_OBJECT;

public:
    enum SourceType { Tag, Artist, User };
    enum Operator { And, Or, AndNot };

    SourceListWidget(int maxSources, QWidget* parent = 0);

    QString rql();
    bool addSource(SourceType type, const QString& name);

private slots:

private:
    typedef QPair<SourceType, QString> Source;

    void setOp(int sourceIdx);
    void setSource(int sourceIdx, QWidget* widget);
    void addPlaceholder();

    static Operator defaultOp(SourceType first, SourceType second);
    static QWidget* createWidget(SourceType type, const QString& name);

    QVBoxLayout* m_layout;
    int m_maxSources;
    QList<Source> m_sources;
};

#endif
