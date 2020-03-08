#include "mainwindow.h"

#include <QApplication>

#include <QString>
#include <QTextStream>
#include "nrtlmanager.h"
#include "nrtlmodel.h"

QString loadTxtFile(const QString& path);


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    a.setApplicationName("Nerve Tracts Lab");
    a.setOrganizationName("Higher School of Economics");
    a.setOrganizationDomain("hse.ru");
    QSurfaceFormat format;
    format.setVersion(2,1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);
    format.setSamples(16);
    QSurfaceFormat::setDefaultFormat(format);
    NrtlModel* md = new NrtlModel();
    NrtlManager::initModel(md);
    MainWindow w;
    w.setModel(md);
    w.showMaximized();
//    a.setStyleSheet(loadTxtFile(":/qss/styles/dark.qss"));
    return a.exec();
}

QString loadTxtFile(const QString& path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QTextStream tin(&file);
    QString result;

    while(!tin.atEnd())
    {
        result += tin.readLine();
 //       result += "\n";
    }

    qDebug() << result;
    return result;
}

