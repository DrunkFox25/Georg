#include "main.hpp"

using namespace std;





int main(int argc, char *argv[]){//todo clean ts up
    //add std::move to the entire codebase as it is lacking with many vector copies
    QApplication app(argc, argv);
    
    QWidget *mainWindow = new QWidget();
    mainWindow->setWindowTitle("Georg");
    mainWindow->resize(1200, 700);

    QLabel *titleLabel = new QLabel(mainWindow);
    QPlainTextEdit *LogDisplay = new QPlainTextEdit(mainWindow);
    QPlainTextEdit *stateIn = new QPlainTextEdit(mainWindow);
    QPushButton *regenButton = new QPushButton(mainWindow);
    QLineEdit *cmdLine = new QLineEdit(mainWindow);
    Interface::GeorgCanvas *canvas = new Interface::GeorgCanvas(mainWindow);
    QScrollArea *displScrollArea = new QScrollArea(mainWindow);

    QGridLayout *layout = new QGridLayout(mainWindow);

    // Span argument: (widget, startRow, startColumn, rowSpan, columnSpan)
    layout->addWidget(titleLabel, 0, 0, 1, 10);
    layout->addWidget(LogDisplay, 1, 5, 9, 5);
    layout->addWidget(stateIn, 1, 0, 9, 5);
    layout->addWidget(regenButton, 10, 0, 1, 3);
    layout->addWidget(cmdLine, 10, 3, 1, 7);
    layout->addWidget(canvas, 11, 0, 5, 5);
    layout->addWidget(displScrollArea, 11, 5, 5, 5);

    regenButton->setText("Regen State");

    titleLabel->setText("Georg - Geogebra 2");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    LogDisplay->setReadOnly(true);
    //LogDisplay->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    //add on select to the the complex display

    stateIn->insertPlainText(QString::fromStdString(getFile("tests/test7.json").str()));//"assets/regenstateex.txt"

    Reader R;
    decentEngine D;

    canvas -> S = &D;

    QObject::connect(regenButton, &QPushButton::clicked, [&](){
        regenButton->setText("Loading New State");
        vector<string> cmds;
        Interface::dispList display;
        R.regenState(stateIn->toPlainText().toStdString(), cmds, canvas->drawer, display, D);
        
        canvas->repaint();//canvas->update() is safer but this is a garentee

        for(auto &cmd : cmds) R.readcmd(cmd, D);

        regenButton->setText("Regen State");
    });

    QObject::connect(cmdLine, &QLineEdit::returnPressed, [&](){
        string cmd = cmdLine->text().toStdString();
        cmdLine->setText("Processing");
        R.readcmd(cmd, D);
        cmdLine->clear();
    });

    mainWindow->show();

    //ofstream Logf("Log.txt");
    Set_Log(new TeeStream(new Interface::QTextEditStream(LogDisplay), &cout));

    Log << "Georg - Geogebra 2\n" << std::flush;

    return app.exec();
}





//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{if you uncomment this it is a dope af rainbow}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}




