#ifndef BUSMONITORWIDGET_H
#define BUSMONITORWIDGET_H

#include <QWidget>

namespace Ui {
class BusMonitorWidget;
}
class BusMonitorModel;

class BusMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BusMonitorWidget(BusMonitorModel &model, QWidget *parent = nullptr);
    ~BusMonitorWidget();

private slots:
    void on_pushButton_clear_clicked();
    void on_rowsInserted(const QModelIndex &parent, int first, int last);

private:
    Ui::BusMonitorWidget *ui;
    BusMonitorModel &_model;
};

#endif // BUSMONITORWIDGET_H
