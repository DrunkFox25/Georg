#include "GeorgEngine.hpp"

#include <QApplication>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPolygon>
#include <QPoint>

using namespace std;


class GeorgCanvas : public QWidget{
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event); 

        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing);

        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(4);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        QBrush brush;
        brush.setColor(Qt::yellow);
        brush.setStyle(Qt::SolidPattern);
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


int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    QWidget mainWindow;
    mainWindow.setWindowTitle("Qt Codespaces Engine");
    mainWindow.resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
    
    QLabel *titleLabel = new QLabel("Georg - Georgebra 2", &mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    QPlainTextEdit *logDisplay = new QPlainTextEdit(&mainWindow);
    logDisplay->setReadOnly(true);
    logDisplay->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    logDisplay->appendPlainText("Log");

    QPlainTextEdit *stateIn = new QPlainTextEdit(&mainWindow);

    QPushButton *regenButton = new QPushButton("Regen State", &mainWindow);

    QLineEdit *cmdLine = new QLineEdit(&mainWindow);

    GeorgCanvas *canvas = new GeorgCanvas();
    canvas->setMinimumSize(300, 200);

    layout->addWidget(titleLabel);
    layout->addWidget(logDisplay);
    layout->addWidget(stateIn);
    layout->addWidget(regenButton);
    layout->addWidget(cmdLine);
    layout->addWidget(canvas);

    QObject::connect(regenButton, &QPushButton::clicked, [&](){
        regenButton->setText("Loading New State");
        string state = logDisplay->toPlainText().toStdString();
        //string out = testing(state);
        //logDisplay->appendPlainText(QString::fromStdString(out));
        regenButton->setText("Regen State");
    });

    QObject::connect(cmdLine, &QLineEdit::returnPressed, [&](){
        string cmd = cmdLine->text().toStdString(); // Retrieve the text
        logDisplay->appendPlainText(QString::fromStdString("ran " + cmd));
    });

    mainWindow.show();

    return app.exec();
}

