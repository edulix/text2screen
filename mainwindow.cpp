#include "mainwindow.h"
#include "helper.h"

#include <QtCore/QVariant>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <qdeclarative.h>
#include <QKeySequence>
#include <QAction>
#include <QSettings>
#include <QApplication>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      mLayout(this),
      mViewer(this),
      mSpeechSelector(this),
      mSpeechWriter(this),
      mHelper(new Helper(this))
{
    setLayout(&mLayout);

    mLayout.addWidget(&mViewer);
    mViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    qmlRegisterType< Helper >("Text2Screen", 1, 0, "Helper");

    QDeclarativeContext *rootContext = mViewer.rootContext();
    rootContext->setContextProperty("helper", QVariant::fromValue(mHelper));

    mViewer.setMainQmlFile(QLatin1String("qml/text2screen/main.qml"));
    mViewer.showExpanded();

    mLayout.setCurrentWidget(&mViewer);

    QAction *action = new QAction(this);
    action->setShortcuts(QKeySequence::Open);
    connect(action, SIGNAL(triggered()), mHelper, SLOT(openSpeechDialog()));
    addAction(action);

    restoreSettings();
}

MainWindow::~MainWindow()
{
    storeSettings();
}

void MainWindow::storeSettings()
{
    // store data

    QSettings settings;
    settings.beginGroup("Main");

    QObject* qmlObject = (QObject*)mViewer.rootObject();

    settings.setValue("lastUsedSpeechText", mHelper->speechText());
    qDebug() << "saving lastUsedSpeechText" << mHelper->speechText().left(10);
    settings.setValue("fontSize", qmlObject->property("fontSize").toInt());
    settings.setValue("contentY", qmlObject->property("contentY").toInt());
    settings.setValue("charsPerSecond", qmlObject->property("charsPerSecond").toInt());

    settings.endGroup();
}

void MainWindow::restoreSettings()
{

    QSettings settings;
    settings.beginGroup("Main");

    QObject* qmlObject = (QObject*)mViewer.rootObject();
    QStringList args = qApp->arguments();

    QString lastUsedSpeechText = settings.value("lastUsedSpeechText", "").toString();
    if (!lastUsedSpeechText.isEmpty()) {
        mHelper->setSpeechText(lastUsedSpeechText);
    } else {
        QString speechPath = settings.value("lastUsedSpeechPath", "").toString();
        settings.endGroup();

        QFile file;
        file.setFileName(speechPath);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // We will use this path to call openSpeechDialog, and user won't be
            // prompted with a dialog because it will directly load it
            file.close();
        } else if (args.length() == 2) {
            file.setFileName(args.at(1));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                // An argument was given, and it's the file to load. Load it!
                file.close();
                speechPath = args.at(1);
            }
        }
        mHelper->openSpeechDialog(speechPath);
    }

    int fontSize = settings.value("fontSize", 0).toInt();
    if (fontSize > 0) {
        qmlObject->setProperty("fontSize", fontSize);
    }

    int contentY = settings.value("contentY", 0).toInt();
    if (contentY != 0) {
        qmlObject->setProperty("contentY", contentY);
    }

    int charsPerSecond = settings.value("charsPerSecond", 0).toInt();
    if (charsPerSecond != 0) {
        qmlObject->setProperty("charsPerSecond", charsPerSecond);
    }

    settings.endGroup();
    settings.sync();
}

void MainWindow::showWidget(const QString &name)
{
    if (name == "viewer") {
        mLayout.setCurrentWidget(&mViewer);
    }
}
