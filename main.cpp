/* подключение заголовочных файлов и классов*/
#include "mainwindow.h"

#include <QApplication>

#include <QString>
#include <QTextStream>
#include "nrtlmanager.h"
#include "nrtlmodel.h"

/*Вызов функцию loadTxtFile, которая принимает адрес объекта класса QString, и после исполнения должна вернуть объект класса QString*/
QString loadTxtFile(const QString& path);


int main(int argc, char *argv[])//Аргумент argc типа integer содержит в себе количество аргументов командной строки.Аргумент argv типа char - указатель на массив строк

{/*Задание формата версии данного виджета*/
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);//установка атрибута AA_UseDesktopOpenGL для объекта QApplication
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts); //установка атрибута AA_ShareOpenGLContexts для объекта QApplication
    QApplication a(argc, argv); //создает новый экземпляр типа QApplication и вызывает конструктор этого класса
    a.setApplicationName("Nerve Tracts Lab");//Устанавливает уникальное имя этого приложения 
    a.setOrganizationName("Higher School of Economics");//Устанавливает уникальное имя домена организации
    a.setOrganizationDomain("hse.ru");//Устанавливает уникальное имя домена организации
    QSurfaceFormat format; //Класс QSurfaceFormat представляет формат QSurface.
    format.setVersion(2,1);//установка формата версии 2.1
    format.setProfile(QSurfaceFormat::CoreProfile);//
    format.setDepthBufferSize(32);//задание буфера  глубины
    format.setSamples(16);// подключение буффера мультисэмпла
    QSurfaceFormat::setDefaultFormat(format);
    NrtlModel* md = new NrtlModel();//создание новой модели
    NrtlManager::initModel(md);
    MainWindow w;
    w.setModel(md);
    w.showMaximized();
//    a.setStyleSheet(loadTxtFile(":/qss/styles/dark.qss"));
    return a.exec();
}

/*Вызов функцию loadTxtFile, которая принимает адрес объекта класса QString, и после исполнения должна вернуть объект класса QString*/
QString loadTxtFile(const QString& path)
{
    QFile file(path);//Класс QFile предоставляет интерфейс для чтения и записи в файлы.
    file.open(QFile::ReadOnly);//открытие файла для чтения
    QTextStream tin(&file);//интерфейс для чтения и записи
    QString result;//предоставление строки символов Unicode

    while(!tin.atEnd())
    {
        result += tin.readLine();
 //       result += "\n";
    }

    qDebug() << result;//Класс QDebug предоставляет выходной поток для отладочной информации.

    return result;// возвращает результат
}

