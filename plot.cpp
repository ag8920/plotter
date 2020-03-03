#include "plot.h"

PlotWidget::PlotWidget(QWidget *parent,const QString &title, bool zoom, bool drag, bool legendview, bool realtime) :
    indexPlot(0)
{
    Q_UNUSED(parent)



    titleText = new QCPTextElement(this,title,QFont("sans",17,QFont::Bold));
    this->plotLayout()->insertRow(0);
    this->plotLayout()->addElement(0,0,titleText);


    this->setInteraction(QCP::iRangeZoom,zoom); //удаление/приближение графика
    this->setInteraction(QCP::iRangeDrag, drag);//перетаскивание графика

    this->setInteraction(QCP::iSelectPlottables);
    this->setInteraction(QCP::iSelectAxes);
    this->setInteraction(QCP::iSelectLegend);

    this->axisRect()->setRangeDrag(Qt::Horizontal);
    this->axisRect()->setRangeDrag(Qt::Vertical);
    this->axisRect()->setRangeZoom(Qt::Horizontal);
    this->axisRect()->setRangeZoom(Qt::Vertical);

    this->legend->setVisible(legendview);
    QFont legendFont = font();
    legendFont.setPointSize(6);
    this->legend->setFont(legendFont);
    this->legend->setSelectedFont(legendFont);
    this->legend->setSelectableParts(QCPLegend::spItems);

    if(realtime)
    {
        QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        this->xAxis->setTicker(timeTicker);
        this->yAxis->setLabel(tr("Значение"));
        this->xAxis->setLabel(tr("Время"));
    }


    this->setLabel();

    connect(this, SIGNAL(selectionChangedByUser()),
            this, SLOT(selectionChanged()));

    connect(titleText, SIGNAL(doubleClicked(QMouseEvent*)),
            this, SLOT(titleDoubleClick(QMouseEvent*)));

    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
            this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));

    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),
            this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    connect(this, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(mousePress()));

    connect(this, SIGNAL(mouseWheel(QWheelEvent*)),
            this, SLOT(mouseWheel()));

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuRequest(QPoint)));

    this->replot();
}


void PlotWidget::setLabel(const QString &xLabel, const QString &yLabel, const QString &name)
{
    this->xAxis->setLabel(xLabel);
    this->yAxis->setLabel(yLabel);
    if(!name.isEmpty()){
        this->plotLayout()->insertRow(0);
        this->plotLayout()->addElement(0, 0, new QCPTextElement(this,
                                                                name,
                                                                QFont("sans", 12, QFont::Bold)));
    }
}

void PlotWidget::add(const QString nameLine, const int widthPx, const QColor color, const Qt::PenStyle style, const QCPScatterStyle::ScatterShape shape)
{
    QPen pen;
    pen.setWidth(widthPx);
    pen.setColor(color);
    pen.setStyle(style);
    this->addGraph();
    this->graph(indexPlot)->setPen(pen);
    if(nameLine.isEmpty())
        this->graph(indexPlot)->setName("Graph_"+QString::number(indexPlot));
    else this->graph(indexPlot)->setName(nameLine);
    this->graph(indexPlot)->setScatterStyle(QCPScatterStyle(shape, widthPx*1.2));
    indexPlot++;

}

void PlotWidget::plot(const int index, const QVector<double> &x, const QVector<double> &y)
{
    this->graph(index)->setData(x,y);
    this->rescaleAxes();
    this->replot();
}

void PlotWidget::plot(const QVector<double> &x, const QVector<double> &y, const QString &name)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    this->addGraph();
    this->graph(indexPlot)->setPen(pen);
    if(name.isEmpty())
        this->graph(indexPlot)->setName("Graph_"+QString::number(indexPlot));
    else this->graph(indexPlot)->setName(name);
    //this->graph(indexPlot)->setScatterStyle(QCPScatterStyle(shape, 2));
    this->graph(indexPlot)->setData(x,y);
    this->rescaleAxes();
    this->replot();
    indexPlot++;

    //    qDebug()<<indexPlot;

}

void PlotWidget::setScatter(bool isPoint,int index,const QCPScatterStyle::ScatterShape shape)
{
    QPen pen = this->graph(index)->pen();
    if (isPoint){
        this->graph(index)->setPen(pen);
        this->graph(index)->setScatterStyle(QCPScatterStyle(shape,pen.width() *2));
    }
    else
        this->graph(index)->setScatterStyle(QCPScatterStyle::ssNone);
    this->replot();

}

void PlotWidget::addPoint(const int index, const double x, const double y)
{
    this->graph(index)->addData(x,y);
    this->rescaleAxes();
    this->replot();
}

void PlotWidget::viewLegend(bool view)
{

    this->legend->setVisible(view);
    this->replot();
}

void PlotWidget::setTextTitle(const QString &text)
{
    titleText->setText(text);
    this->replot();
}



void PlotWidget::realtimeData(const int index, const double data)
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    if(index<=indexPlot){
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
        static double lastPointKey = 0;
        if(1)// (key-lastPointKey > 0.002) // at most add point every 2 ms
        {
            // add data to lines:
            this->graph(index)->addData(key,data);

            // rescale value (vertical) axis to fit the current data:
            //this->graph(index)->rescaleValueAxis();
            lastPointKey = key;
        }
        // make key axis range scroll with the data (at a constant range size of 8):

        //this->xAxis->setRange(key, 8, Qt::AlignRight);
        //this->yAxis->setRange(key, 8, Qt::AlignRight);
        this->replot();
    }

}

void PlotWidget::titleDoubleClick(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
    {
        bool ok;
        QString newTitle = QInputDialog::getText(this, "Input Title", "New plot title:", QLineEdit::Normal, title->text(), &ok);
        if (ok)
        {
            setTextTitle(newTitle);
        }
    }
}

void PlotWidget::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    if (part == QCPAxis::spAxisLabel)
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "Input axis name", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok)
        {
            axis->setLabel(newLabel);
            this->replot();
        }
    }
}

void PlotWidget::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    Q_UNUSED(legend)
    if (item)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "Input legend name", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            this->replot();
        }
    }
}

void PlotWidget::selectionChanged()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    for (int i=0; i<this->graphCount(); ++i)
    {
        QCPGraph *graph = this->graph(i);
        QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

void PlotWidget::mousePress()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeDrag(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeDrag(this->yAxis->orientation());
    else
        this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void PlotWidget::mouseWheel()
{
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeZoom(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeZoom(this->yAxis->orientation());
    else
        this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void PlotWidget::removeSelectedGraph()
{
    if (this->selectedGraphs().size() > 0)
    {
        this->removeGraph(this->selectedGraphs().first());
        this->replot();
        this->indexPlot--;
    }
}

void PlotWidget::removeAllGraphs()
{
    this->clearGraphs();
    this->replot();
    this->indexPlot = 0;
}

void PlotWidget::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (this->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    } else  // general context menu on graphs requested
    {
        if (this->selectedGraphs().size() > 0)
            menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        if (this->graphCount() > 0)
            menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }

    menu->popup(this->mapToGlobal(pos));
}

void PlotWidget::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            this->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            this->replot();
        }
    }
}





