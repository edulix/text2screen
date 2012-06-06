#ifndef HELPER_H
#define HELPER_H

#include "qmlapplicationviewer.h"

#include <QObject>
#include <QMetaType>

class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(int argc, char *argv[], QmlApplicationViewer &viewer);
    explicit Helper(const Helper &copy);
    explicit Helper() : QObject(0), mApplicationViewer(0) {}

public Q_SLOTS:
    /**
     * @returns the text of the speech given to the app as an argument
     */
    Q_INVOKABLE QString getSpeechText();

    Q_INVOKABLE void toggleFullScreen();

protected:
    QString mSpeechText;
    QmlApplicationViewer *mApplicationViewer;
};

Q_DECLARE_METATYPE(Helper*)

#endif // HELPER_H
