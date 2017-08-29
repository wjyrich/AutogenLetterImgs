#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <ctime>

#include "graphicssceneex.h"
#include "graphicsviewex.h"
#include "extcolordefs.h"
#include "text.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QGraphicsPixmapItem *pixmapItem;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QRect getActualRectangle();
    QImage *genImg(QString &tagSource,QString filenameTemplate="",bool massGen=false);

public slots:
    void generateBtnClicked();
    void massGenerateBtnClicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
