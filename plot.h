#ifndef PLOT_H
#define PLOT_H

#include "qcustomplot.h"
#include <QObject>
#include <QWidget>

class PlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr,const QString &title="", bool zoom=true, bool drag=true, bool legendview=true, bool realtime=false);
    void setLabel(const QString &xLabel="X", const QString &yLabel="Y",const QString &name="");
    void add(const QString nameLine="", const int widthPx=1,
                         const QColor color=Qt::black, const Qt::PenStyle style=Qt::SolidLine,
                         const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssDot);

    void plot(const int index, const QVector<double> &x, const QVector<double> &y);
    void plot(const QVector<double> &x, const QVector<double> &y, const QString &name="");
//    void setRange()
    void setScatter(bool isPoint, int index=0,
              const QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssDot);
    void addPoint(const int index,const double x,const double y);
    void viewLegend(bool view);
    void setTextTitle(const QString &text);
public slots:
    void realtimeData(const int index, const double data);

private slots:
    void titleDoubleClick(QMouseEvent *event);
    void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
private:
    int indexPlot;
    QVector <QVector<QVariant>> vdata;
    QCPTextElement *titleText;
};

#endif // PLOT_H
