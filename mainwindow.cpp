#include "mainwindow.h"
#include "helper.h"

#include <QtCore/QVariant>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <qdeclarative.h>
#include <QKeySequence>
#include <QStringListModel>
#include <QAction>
#include <QSettings>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QCompleter>
#include <QBoxLayout>
#include <QListView>
#include <QModelIndex>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      mLayout(this),
      mViewer(this),
      mSpeechSelector(this),
      mSpeechListView(this),
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

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(action, SIGNAL(triggered()), this, SLOT(toggleSpeechSelector()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::Key_E));
    connect(action, SIGNAL(triggered()), this, SLOT(toggleSpeechWriter()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::Key_N));
    connect(action, SIGNAL(triggered()), this, SLOT(newSpeechText()));
    addAction(action);


    action = new QAction(this);
    action->setShortcut(QKeySequence::Save);
    connect(action, SIGNAL(triggered()), mHelper, SLOT(save()));
    addAction(action);

    restoreSettings();


    mSpeechSelectorContainer = new QWidget(this);
    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, mSpeechSelectorContainer);
    boxLayout->addWidget(&mSpeechSelector);
    mSpeechSelector.installEventFilter(this);

    mStringListModel = new QStringListModel(mHelper->speechTitles());
    mFilterModel = new SortFilterProxyModel(this);
    mFilterModel->setSourceModel(mStringListModel);

    mSpeechSelector.setObjectName("SpeechSelector");
    connect(&mSpeechSelector, SIGNAL(textChanged(QString)), this, SLOT(changeFilter(QString)));
    mSpeechListView.setModel(mFilterModel);
    mSpeechListView.setObjectName("SpeechListView");
    mSpeechListView.setSelectionMode(QAbstractItemView::SingleSelection);
    mSpeechListView.setFocusPolicy(Qt::NoFocus);
    mSpeechListView.selectionModel()->select(mFilterModel->index(0, 0), QItemSelectionModel::SelectCurrent);
    boxLayout->addWidget(&mSpeechListView);
    mLayout.addWidget(mSpeechSelectorContainer);


    mSpeechWriter.setObjectName("SpeechWriter");
    mLayout.addWidget(&mSpeechWriter);

    connect(mHelper, SIGNAL(speechItemsChanged()), this, SLOT(reloadSpeechItems()));

}

MainWindow::~MainWindow()
{
    storeSettings();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        qDebug() << "keyEvent->key() " << keyEvent->key();
        if (keyEvent->key() == Qt::Key_Up) {
            selectUp();
            return true;
        } else if (keyEvent->key() == Qt::Key_Down) {
            selectDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_Return) {
            QModelIndex currentIndex = mSpeechListView.selectionModel()->selectedIndexes().first();
            mHelper->setSpeechTitle(currentIndex.data().toString());
            toggleSpeechSelector();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void MainWindow::storeSettings()
{
    // store data

    QSettings settings;
    settings.beginGroup("Main");

    QObject* qmlObject = (QObject*)mViewer.rootObject();

    settings.setValue("lastUsedSpeechText", mHelper->speechText());
    settings.setValue("lastUsedSpeechTitle", mHelper->speechTitle());
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

    QFile file;
    QString speechPath = settings.value("lastUsedSpeechPath", "").toString();
    file.setFileName(speechPath);
    bool autoOpen = false;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // We will use this path to call openSpeechDialog, and user won't be
        // prompted with a dialog because it will directly load it
        file.close();
        autoOpen = true;
    } else if (args.length() == 2) {
        file.setFileName(args.at(1));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // An argument was given, and it's the file to load. Load it!
            file.close();
            autoOpen = true;
            speechPath = args.at(1);
        }
    }

    QString lastUsedSpeechText = settings.value("lastUsedSpeechText", "").toString();
    if (autoOpen) {
        mHelper->openSpeechDialog(speechPath);
        QString speechTitle = settings.value("lastUsedSpeechTitle", "").toString();
        if (file.open(QIODevice::ReadOnly | QIODevice::Text) && !speechTitle.isEmpty()) {
            mHelper->setSpeechTitle(speechTitle);
        }
    } else if (!lastUsedSpeechText.isEmpty()) {
        mHelper->setSpeechText(lastUsedSpeechText);
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

void MainWindow::toggleSpeechSelector()
{
    if (mLayout.currentWidget() == mSpeechSelectorContainer) {
        mLayout.setCurrentWidget(&mViewer);

    } else if (mLayout.currentWidget() == &mSpeechWriter) {
        QString text = mSpeechWriter.toPlainText();
        if (text.isEmpty()) {
            mHelper->deleteSpeechItem(mHelper->speechTitle());
        } else {
            QString html = mSpeechWriter.toPlainText();
            QStringList strList = html.split("\n");
            bool found = false;
            Q_FOREACH(QString line, strList) {
                if (line.startsWith("=")) {
                    QString lineCopy = line;
                    QString title = lineCopy.replace("=", "").trimmed();
                    mHelper->changeSpeechTitle(title);
                    QString text = html.replace(line, "");
                    mHelper->setSpeechText(text);
                    found = true;
                    break;
                }
            }
            if (!found) {
                mHelper->setSpeechText(html);
            }
        }
        mLayout.setCurrentWidget(&mViewer);
    } else {
        int indexSpeechTitle = mHelper->speechTitles().indexOf(mHelper->speechTitle());
        QModelIndex index = mFilterModel->index(indexSpeechTitle, 0);
        mSpeechListView.selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

        mLayout.setCurrentWidget(mSpeechSelectorContainer);
        mSpeechSelector.setFocus(Qt::MouseFocusReason);
    }

}

void MainWindow::toggleSpeechWriter()
{
    if (mLayout.currentWidget() != &mSpeechWriter) {
        mSpeechWriter.setText("== " + mHelper->speechTitle() + " ==\n" + mHelper->speechText());
        mLayout.setCurrentWidget(&mSpeechWriter);
    }
}

void MainWindow::selectUp()
{
    if (mSpeechListView.selectionModel()->selectedIndexes().isEmpty()) {
        mSpeechListView.selectionModel()->select(
            mSpeechListView.model()->index(0,0), QItemSelectionModel::SelectCurrent);
        return;
    }
    QModelIndex currentIndex = mSpeechListView.selectionModel()->selectedIndexes().first();
    QModelIndex upIndex = currentIndex.sibling(currentIndex.row() - 1, currentIndex.column());
    if (!upIndex.isValid()) {
        return;
    }
    mSpeechListView.selectionModel()->select(upIndex, QItemSelectionModel::SelectCurrent);
}

void MainWindow::selectDown()
{
    if (mSpeechListView.selectionModel()->selectedIndexes().isEmpty()) {
        mSpeechListView.selectionModel()->select(
            mSpeechListView.model()->index(0,0), QItemSelectionModel::SelectCurrent);
        return;
    }
    QModelIndex currentIndex = mSpeechListView.selectionModel()->selectedIndexes().first();
    QModelIndex downIndex = currentIndex.sibling(currentIndex.row() + 1, currentIndex.column());
    if (!downIndex.isValid()) {
        return;
    }
    mSpeechListView.selectionModel()->select(downIndex, QItemSelectionModel::SelectCurrent);
}

void MainWindow::changeFilter(const QString &filterString)
{
    QModelIndex currentIndex;
    if (!mSpeechListView.selectionModel()->selectedIndexes().isEmpty()) {
        QModelIndex currentIndex = mSpeechListView.selectionModel()->selectedIndexes().first();
        currentIndex = mFilterModel->mapToSource(currentIndex);
    }

    mFilterModel->setFilterString(filterString);

    if (currentIndex.isValid()) {
        currentIndex = mFilterModel->mapFromSource(currentIndex);
    }

    if (!currentIndex.isValid() && mFilterModel->rowCount() > 0) {
        currentIndex = mFilterModel->index(0, 0);
        mSpeechListView.selectionModel()->select(currentIndex, QItemSelectionModel::SelectCurrent);
    }
}

void MainWindow::reloadSpeechItems()
{
    mStringListModel->setStringList(mHelper->speechTitles());
}

void MainWindow::newSpeechText()
{
    QString defaultTitle = tr("speech");
    QString newTitle = defaultTitle;
    int n = 1;
    QStringList titles = mHelper->speechTitles();

    while(titles.contains(newTitle)) {
        newTitle = defaultTitle + " " + QString::number(n);
    }
    mHelper->appendSpeech(newTitle, "...");
    mHelper->setSpeechTitle(newTitle);

    mSpeechWriter.setText("== " + mHelper->speechTitle() + " ==\n" + mHelper->speechText());
    mLayout.setCurrentWidget(&mSpeechWriter);
}
