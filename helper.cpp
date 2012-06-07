#include "helper.h"

#include "mainwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QSettings>

Helper::Helper(MainWindow *mainWindow)
    : QObject(0)
{
    mMainWindow = mainWindow;
    toggleFullScreen();
}

Helper::Helper(const Helper &copy)
    : mMainWindow(0)
{
    mSpeechText = copy.mSpeechText;
    mMainWindow = copy.mMainWindow;
}

QString Helper::speechText() const
{
    return mSpeechText;
}

void Helper::setSpeechText(const QString &speechText)
{
    mSpeechText = speechText;

    QSettings settings;
    settings.beginGroup("Main");
    settings.setValue("lastUsedSpeechText", mSpeechText);
    settings.endGroup();
    settings.sync();

    qDebug() << "\n\n\nemit speechTextChanged();" << mSpeechText.left(10);
    emit speechTextChanged();
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
        lastUsedSpeechPath = QFileDialog::getOpenFileName(mMainWindow, tr("Open Speech"),
            lastUsedSpeechPath);
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

            lastUsedSpeechPath = QFileDialog::getOpenFileName(mMainWindow, tr("Open Speech"),
                "");
            file.setFileName(lastUsedSpeechPath);
        }
    }

    setSpeechText(QString::fromUtf8(file.readAll()));
}
