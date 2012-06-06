#include "qmlapplicationviewer.h"
#include "helper.h"

#include <QtGui/QApplication>
#include <QtCore/QVariant>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <qdeclarative.h>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);


    qmlRegisterType< Helper >("Text2Screen", 1, 0, "Helper");
    QDeclarativeContext *rootContext = viewer.rootContext();
    rootContext->setContextProperty("helper", QVariant::fromValue(new Helper(argc, argv, viewer)));

    viewer.setMainQmlFile(QLatin1String("qml/text2screen/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
