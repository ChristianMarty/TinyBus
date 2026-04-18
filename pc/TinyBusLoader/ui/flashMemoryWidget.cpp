#include "flashMemoryWidget.h"
#include "ui_flashMemoryWidget.h"

FlashMemoryWidget::FlashMemoryWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FlashMemoryWidget)
{
    ui->setupUi(this);
}

FlashMemoryWidget::~FlashMemoryWidget()
{
    delete ui;
}

void FlashMemoryWidget::update(const QuCLib::HexFileParser &hexFile)
{
    ui->textEdit_data->clear();
    if(hexFile.binary().empty()) return;

    QuCLib::HexFileParser::BinaryChunk data = hexFile.binary().first();

    ui->textEdit_data->append("                   00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F");
    ui->textEdit_data->append("                   ................................................");

    for(uint16_t i = 0; i<data.data.length(); i+=16){
        QString line = QString::number(i).rightJustified(5,' ')+" / ";
        line += QString::number(data.offset+i,16).toUpper().rightJustified(4,'0').prepend("0x");
        line += "  : ";
        for(uint16_t j = 0; j<16; j++){

            uint16_t offset = i+j;
            if(offset >= data.data.length()) break;

            uint8_t byte = data.data.at(offset);
            line +=" "+QString::number(byte,16).toUpper().rightJustified(2,'0');
            if(j == 7)line +=" ";
        }

        ui->textEdit_data->append(line);
    }
}
