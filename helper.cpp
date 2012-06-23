#include "helper.h"

#include "mainwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QSettings>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>

Helper::Helper(MainWindow *mainWindow)
    : QObject(0)
{
    mMainWindow = mainWindow;
    toggleFullScreen();
}

Helper::Helper(const Helper &copy)
    : QObject(copy.parent()), mMainWindow(0)
{
    mSpeechTitle = copy.mSpeechTitle;
    mSpeechItems = copy.mSpeechItems;
    mSpeechTitles = copy.mSpeechTitles;
    mMainWindow = copy.mMainWindow;
}

QString Helper::speechText() const
{
    if (mSpeechTitle.isEmpty()) {
        return QString();
    }

    return mSpeechItems[mSpeechTitle];
}

void Helper::setSpeechText(const QString &speechText)
{
    if(mSpeechTitle.isEmpty()) {
        qDebug() << "!!! setSpeechText called and mSpeechTitle.isEmpty()";
        return;
    }

    if (speechText == mSpeechItems[mSpeechTitle]) {
        return;
    }

    mSpeechItems[mSpeechTitle] = speechText;

    QSettings settings;
    settings.beginGroup("Main");
    settings.setValue("lastUsedSpeechText", mSpeechItems[mSpeechTitle]);
    settings.endGroup();
    settings.sync();

    emit speechTextChanged();
}

QString Helper::speechTitle() const
{
    return mSpeechTitle;
}

void Helper::setSpeechTitle(const QString &speechTitle)
{
    if (!mSpeechItems.contains(speechTitle)) {
        qDebug() << "!!! setSpeechTitle called and !mSpeechItems.contains(speechTitle)";
        return;
    }

    QString oldSpeechText = speechText();

    mSpeechTitle = speechTitle;

    QSettings settings;
    settings.beginGroup("Main");
    settings.setValue("lastUsedSpeechTitle", mSpeechTitle);
    settings.setValue("lastUsedSpeechText", mSpeechItems[mSpeechTitle]);
    settings.endGroup();
    settings.sync();

    emit speechTitleChanged();

    if (oldSpeechText != mSpeechItems[mSpeechTitle]) {
        emit speechTextChanged();
    }
}

void Helper::changeSpeechTitle(const QString &speechTitle)
{
    QString oldTitle = mSpeechTitle;
    mSpeechItems[speechTitle] = mSpeechItems[mSpeechTitle];

    mSpeechItems.remove(oldTitle);

    int pos = mSpeechTitles.indexOf(oldTitle);
    if (pos == -1) {
        mSpeechTitles.append(speechTitle);
    } else {
        mSpeechTitles.replace(pos, speechTitle);
    }

    mSpeechTitle = speechTitle;

    emit speechTitleChanged();
    emit speechItemsChanged();
}

void Helper::toggleFullScreen()
{
    if (!mMainWindow->isFullScreen()) {
        mMainWindow->showFullScreen();
    } else {
        mMainWindow->showNormal();
    }
}

void Helper::openSpeechDialog(const QString &tryThisFirst)
{
    QString lastUsedSpeechPath = tryThisFirst;
    QFile file(lastUsedSpeechPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        lastUsedSpeechPath = QFileDialog::getOpenFileName(mMainWindow,
            tr("Open Speech"), lastUsedSpeechPath);
        file.setFileName(lastUsedSpeechPath);

        while(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox messageBox(mMainWindow);
            messageBox.setText(tr("Sorry, the file couldn't be opened for read"));
            messageBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            int ret = messageBox.exec();

            if (ret == QMessageBox::Cancel) {
                qDebug() << "User cancelled opening the file";
                return;
            }

            lastUsedSpeechPath = QFileDialog::getOpenFileName(mMainWindow,
                tr("Open Speech"), "");
            file.setFileName(lastUsedSpeechPath);
        }
    }

    QSettings settings;
    settings.beginGroup("Main");
    settings.setValue("lastUsedSpeechPath", lastUsedSpeechPath);
    settings.endGroup();
    settings.sync();

    // erase all!
    mSpeechItems = QHash<QString, QString>();
    mSpeechTitles = QStringList();
    mSpeechTitle = QString();

    QString key, value;
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine());
        if (line.startsWith("=")) {
            if (!value.isEmpty() && !key.isEmpty()) {
                line = line.replace("=", "").trimmed();
                mSpeechItems[key] = value;
                mSpeechTitles.append(key);

                if (mSpeechTitle.isEmpty()) {
                    setSpeechTitle(key);
                }
            }

            key = line.replace("=", "").trimmed();
            value = QString();
        } else {
            value += line;
        }
    }

    if (!value.isEmpty() && !key.isEmpty()) {
        mSpeechItems[key] = value;
        mSpeechTitles.append(key);

        if (mSpeechTitle.isEmpty()) {
            setSpeechTitle(key);
        }
    }

    emit speechItemsChanged();
}

void Helper::deleteSpeechItem(const QString &speechTitle)
{
    if (!mSpeechItems.contains(speechTitle) || mSpeechTitles.count() == 1) {
        return;
    }

    int titlesIndex = mSpeechTitles.indexOf(speechTitle);


    mSpeechItems.remove(speechTitle);
    mSpeechTitles.removeOne(speechTitle);

    if (mSpeechTitle == speechTitle) {
        int nextIndex;
        if (titlesIndex == 0) {
            nextIndex = titlesIndex;
        } else {
            nextIndex = titlesIndex - 1;
        }

        QString nextTitle = mSpeechTitles.at(nextIndex) ;

        setSpeechTitle(nextTitle);
    }
    emit speechItemsChanged();
}

void Helper::save()
{
    qDebug() << "file saved!";
    QSettings settings;
    settings.beginGroup("Main");
    QString lastUsedSpeechPath = settings.value("lastUsedSpeechPath", "").toString();
    settings.endGroup();

    QFile file(lastUsedSpeechPath);

    while(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        lastUsedSpeechPath = QFileDialog::getSaveFileName(mMainWindow,
            tr("Save Speech"), lastUsedSpeechPath);
        file.setFileName(lastUsedSpeechPath);
    }

    settings.setValue("lastUsedSpeechPath", lastUsedSpeechPath);
    settings.endGroup();
    settings.sync();

    QString text;
    Q_FOREACH(QString title, mSpeechTitles) {
        text += "== " + title + " ==\n" + mSpeechItems[title];
    }

    QTextStream out(&file);
    out << text;
    file.close();
    qDebug() << "file saved!";
}

void Helper::appendSpeech(const QString &title, const QString &text)
{
    mSpeechItems[title] = text;
    mSpeechTitles.append(title);

    emit speechItemsChanged();
}
