#include "MainFrame.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);

    QTranslator translator;
    translator.load(":/zh.qm", QApplication::applicationDirPath());
    a.installTranslator(&translator);

    QFile qssFile(":/stylesheet.qss");
    qssFile.open(QIODevice::ReadOnly);
    QString stylesheet = QString::fromUtf8(qssFile.readAll());
    a.setStyleSheet(stylesheet);
    qssFile.close();

    MainFrame w;
    w.setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    w.show();

    return a.exec();
}
