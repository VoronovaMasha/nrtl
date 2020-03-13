#include "mainwindow.h" // подключение заголовочных файлов и классов

#include <QApplication>

#include <QString>
#include <QTextStream>
#include "nrtlmanager.h"
#include "nrtlmodel.h"

QString loadTxtFile(const QString& path); //Вызов функцию loadTxtFile, принимающей адрес объекта класса QString, и после исполнения возвращает объект класса QString

/*Создание данного виджета и формата для него.*/
int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    a.setApplicationName("Nerve Tracts Lab");// задание уникального имени приложения
    a.setOrganizationName("Higher School of Economics"); // задание уникального имени домена
    a.setOrganizationDomain("hse.ru");// задание уникального имени домена
    QSurfaceFormat format; //предоставление формата  QSurfaceFormat
    format.setVersion(2,1); //задание версии 
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32); //задание буфера глубины
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
 //Вызов функцию loadTxtFile, принимающей адрес объекта класса QString, и после исполнения возвращает объект класса QString
QString loadTxtFile(const QString& path)
    //открытие файла в интерфейсе только на чтение
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

    qDebug() << result; // вывод информации об отладке
    return result; // возвращение результата
}

