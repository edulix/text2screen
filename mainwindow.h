#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qmlapplicationviewer.h"

#include <QWidget>
#include <QStackedLayout>
#include <QComboBox>
#include <QTextEdit>

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

protected:
    QStackedLayout mLayout;
    QmlApplicationViewer mViewer;
    QComboBox mSpeechSelector;
    QTextEdit mSpeechWriter;
    Helper *mHelper;
};

#endif // MAINWINDOW_H
