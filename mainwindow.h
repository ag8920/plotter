#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "plot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QCPGraph *graphic;


    PlotWidget *pwgt;

    int TimeElapsed;
    QVector<double>x,y;
    QVector<double>x2,y2;
    QTimer *playBackTimer;
};

#endif // MAINWINDOW_H
