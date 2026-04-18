#ifndef FLASHMEMORYWIDGET_H
#define FLASHMEMORYWIDGET_H

#include <QWidget>
#include "../QuCLib/source/hexFileParser.h"

namespace Ui {
class FlashMemoryWidget;
}

class FlashMemoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlashMemoryWidget(QWidget *parent = nullptr);
    ~FlashMemoryWidget();

    void update(const QuCLib::HexFileParser &hexFile);

private:
    Ui::FlashMemoryWidget *ui;
};

#endif // FLASHMEMORYWIDGET_H
