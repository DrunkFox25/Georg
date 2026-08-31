#include "GeorgEngine.hpp"

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>


using namespace std;

int main(int argc, char *argv[]) {
    // 1. Initialize the GUI application context
    QApplication app(argc, argv);

    // 2. Create a main window widget
    QWidget mainWindow;
    mainWindow.setWindowTitle("Qt Codespaces Engine");
    mainWindow.resize(400, 300);

    // 3. Create structural layouts and interactive widgets
    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
    
    QLabel *titleLabel = new QLabel("Hello from Native Qt6!", &mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    
    QPushButton *actionButton = new QPushButton("Click Me", &mainWindow);
    actionButton->setStyleSheet("padding: 10px; background-color: #3498db; color: white; border-radius: 5px;");

    layout->addWidget(titleLabel);
    layout->addWidget(actionButton);

    // Connect button click event to a lambda routine
    QObject::connect(actionButton, &QPushButton::clicked, [&]() {
        titleLabel->setText("It Works! Button Triggered.");
        actionButton->setText("Clicked!");
    });

    mainWindow.show();

	//testing();

    return app.exec();
}

