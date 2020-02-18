#include "mainwindow.h"

#include <QApplication>

#include <QString>
#include <QTextStream>
#include "nrtlmanager.h"
#include "nrtlmodel.h"

QString loadTxtFile(const QString& path);


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setVersion(2,1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);
    format.setSamples(16);
    QSurfaceFormat::setDefaultFormat(format);
    MainWindow w;
    NrtlModel* md = new NrtlModel();
    NrtlManager::initModel(md);
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

