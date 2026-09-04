#pragma once

#include "main.hpp"

struct Interface{

    struct GeorgCanvas : public QWidget{
        using QWidget::QWidget;

        void paintEvent(QPaintEvent *event) override {
            Q_UNUSED(event); 

            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            QPen pen;
            QBrush brush;

            pen.setColor(Qt::blue);
            pen.setWidth(4);
            pen.setStyle(Qt::SolidLine);

            brush.setColor(Qt::yellow);
            brush.setStyle(Qt::SolidPattern);

            painter.setPen(pen);//this sends it to gpu
            painter.setBrush(brush);

            painter.drawLine(10, 10, 380, 10);
            painter.drawRect(20, 40, 150, 100);

            brush.setColor(Qt::red);
            painter.setBrush(brush);

            painter.drawEllipse(220, 40, 150, 100);

            QPolygon triangle;
            triangle << QPoint(200, 160)   // Top point
                        << QPoint(100, 260)   // Bottom-left point
                        << QPoint(300, 260);  // Bottom-right point
            
            brush.setColor(Qt::green);
            painter.setBrush(brush);
            painter.drawPolygon(triangle);

            return;
        }
    };
};

