#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <windows.h>
#include <stdlib.h>

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Q_UNUSED(context);
    QFile log("debug_log.txt");
    if (!log.open(QIODevice::WriteOnly | QIODevice::Append)) return;
    QTextStream out(&log);
    QString level;
    switch (type) {
        case QtDebugMsg: level = "DEBUG"; break;
        case QtWarningMsg: level = "WARN"; break;
        case QtCriticalMsg: level = "CRIT"; break;
        case QtFatalMsg: level = "FATAL"; break;
        case QtInfoMsg: level = "INFO"; break;
    }
    out << "[" << level << "] " << msg << "\n";
    out.flush();
    log.close();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    int argc = __argc;
    char **argv = __argv;

    qInstallMessageHandler(messageHandler);

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
