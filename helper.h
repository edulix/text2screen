#ifndef HELPER_H
#define HELPER_H

#include "qmlapplicationviewer.h"

#include <QObject>
#include <QMetaType>
#include <QHash>

class MainWindow;

class Helper : public QObject
{
    Q_OBJECT
    // By default, the first one appearing on the file
    Q_PROPERTY(QString speechText READ speechText() WRITE setSpeechText NOTIFY speechTextChanged)
    // By default, the first one appearing on the file
    Q_PROPERTY(QString speechTitle READ speechTitle() WRITE setSpeechTitle NOTIFY speechTitleChanged)
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

    Q_INVOKABLE QString speechTitle() const;

    Q_INVOKABLE void setSpeechTitle(const QString &speechTitle);

    Q_INVOKABLE void changeSpeechTitle(const QString &speechTitle);

    Q_INVOKABLE void toggleFullScreen();

    Q_INVOKABLE void openSpeechDialog(const QString &tryThisFirst = "");

    QHash<QString, QString> speechItems() const { return mSpeechItems; }

    QStringList speechTitles() const { return mSpeechTitles; }

    void save();

    void deleteSpeechItem(const QString &speechTitle = QString());

    void appendSpeech(const QString &title, const QString &text);

Q_SIGNALS:
    void speechTextChanged();
    void speechTitleChanged();

    void speechItemsChanged();

protected:
    QString mSpeechTitle;
    MainWindow *mMainWindow;
    QHash<QString, QString> mSpeechItems;
    QStringList mSpeechTitles;
};

Q_DECLARE_METATYPE(Helper*)

#endif // HELPER_H
