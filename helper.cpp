#include "helper.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

Helper::Helper(int argc, char *argv[], QmlApplicationViewer &viewer)
    : QObject(0)
{
    mApplicationViewer = &viewer;

    QFile file;

    if (argc != 2) {
        QString fileName = QFileDialog::getOpenFileName(mApplicationViewer, tr("Open File"),
            "");
        file.setFileName(fileName);
    } else {
        file.setFileName(argv[1]);
    }


    while(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox messageBox;
        messageBox.setText(tr("Sorry, the file couldn't be opened for read"));
        messageBox.exec();

        QString fileName = QFileDialog::getOpenFileName(mApplicationViewer, tr("Open File"),
            "");
        file.setFileName(fileName);
    }

    mSpeechText = QString::fromUtf8(file.readAll());
}

Helper::Helper(const Helper &copy)
{
    mSpeechText = copy.mSpeechText;
    mApplicationViewer = copy.mApplicationViewer;
}

QString Helper::getSpeechText()
{
    return mSpeechText;
}
