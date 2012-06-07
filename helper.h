#ifndef HELPER_H
#define HELPER_H

#include "qmlapplicationviewer.h"

#include <QObject>
#include <QMetaType>

class MainWindow;

class Helper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString speechText READ speechText() WRITE setSpeechText NOTIFY speechTextChanged)
public:
    explicit Helper(MainWindow *mainWindow);
    explicit Helper(const Helper &copy);
    explicit Helper() : QObject(0), mMainWindow(0) {}

public Q_SLOTS:
    /**
     * @returns the text of the speech given to the app as an argument
     */
    Q_INVOKABLE QString speechText() const;

    Q_INVOKABLE void setSpeechText(const QString &speechText);

    Q_INVOKABLE void toggleFullScreen();

    Q_INVOKABLE void openSpeechDialog(const QString &tryThisFirst = "");

Q_SIGNALS:
    void speechTextChanged();

protected:
    QString mSpeechText;
    MainWindow *mMainWindow;
};

Q_DECLARE_METATYPE(Helper*)

#endif // HELPER_H
