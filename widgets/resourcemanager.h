#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QWidget>
#include <QTreeWidget>
#include "nrtlmanager.h"

class ResourceManager : public QWidget
{
    Q_OBJECT
public:
    explicit ResourceManager(QWidget *parent = nullptr);
signals:

public slots:

    void update();

private:
    QTreeWidget* treeWgt;
};

#endif // RESOURCEMANAGER_H
