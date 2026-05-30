#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QWidget>

class ColorPalette
{
public:

    static QPalette ok(void){
        QPalette palette;
        QBrush brush(QColor(0, 204, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(_colorDisabled));
        return palette;
    };

    static QPalette warning(void){
        QPalette palette;
        QBrush brush(QColor(213, 142, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(_colorDisabled));
        return palette;
    };

    static QPalette error(void){
        QPalette palette;
        QBrush brush(QColor(255, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(_colorDisabled));
        return palette;
    };

    static QPalette status(void){
        QPalette palette;
        QBrush brush(QColor(0, 51, 204, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(_colorDisabled));
        return palette;
    };

private:
    constexpr static const QColor _colorDisabled{180, 180, 180, 255};
};

#endif // COLORPALETTE_H
