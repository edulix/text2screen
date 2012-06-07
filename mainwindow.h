#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qmlapplicationviewer.h"
#include "sortfilterproxymodel.h"

#include <QWidget>
#include <QStackedLayout>
#include <QTextEdit>
#include <QListView>
#include <QLineEdit>

class Helper;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    QmlApplicationViewer & viewer() { return mViewer; }

public Q_SLOTS:
    void showWidget(const QString &name);

    void storeSettings();

    void restoreSettings();

    void toggleSpeechSelector();

    void toggleSpeechWriter();

protected Q_SLOTS:
    void selectUp();

    void selectDown();

    void changeFilter(const QString &filterString);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected:
    QStackedLayout mLayout;
    QmlApplicationViewer mViewer;
    QLineEdit mSpeechSelector;
    QListView mSpeechListView;
    QWidget *mSpeechSelectorContainer;
    SortFilterProxyModel *mFilterModel;
    QTextEdit mSpeechWriter;
    Helper *mHelper;
};

#endif // MAINWINDOW_H
