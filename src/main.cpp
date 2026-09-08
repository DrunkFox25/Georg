

//clean everything up
//redo all includes
//add std::move to the entire codebase as it is lacking with many vector copies
//todo: make log in a way where it is Log(label) <<; so that it can be filtered
//make loggerbert the logger
//rewrite tests
//try to remove stds from cpp files
//rewrite reader
//make using functions, eg. using sin, or even groups eg. using trig

/*Far far future: EMTBW - Extenable Math Text Box Widget
math text editor
as smooth as desmos
with selection that can be switched to latex or plain text
and can output AST tree or just a plaintext version
that can do desmos shit via custom shorthand ie. pi-> pi symbol but I can define my own symobols (auto format module)
and it's all in a QT widget
base it off a YAWYSIWYGEE fork, so maintain all it's features to
for tex rendering use https://github.com/NanoMichael/MicroTex

and at that point it's a seperate repo
*/


//implement some of those cool desmos features like the audio and color
//make 3d one day

//for parsing with https://github.com/JohnDTill/YAWYSIWYGEE for easy typing but with a switch for latex typing
//also make importer for exquations via YAWYSIWYGEE and Qt6::Xml -> xml files cool shit
//this fucker: YAWYSIWYGEE is an acronym
//https://johndtill.github.io/YAWYSIWYGEE_WASM/


/*
https://doc.qt.io/qt-6/gallery.html
https://doc.qt.io/qt-6/graphicsview.html
https://doc.qt.io/qt-6/qgraphicsscene.html
https://doc.qt.io/qt-6/qgraphicsview.html
https://doc.qt.io/qt-6/qgraphicsitem.html
*/



int main(int argc, char *argv[]){
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
        std::vector<std::string> cmds;
        Interface::dispList display;
        R.regenState(stateIn->toPlainText().toStdString(), cmds, canvas->drawer, display, D);
        
        canvas->repaint();//canvas->update() is safer but this is a garentee

        for(auto &cmd : cmds) R.readcmd(cmd, D);

        regenButton->setText("Regen State");
    });

    QObject::connect(cmdLine, &QLineEdit::returnPressed, [&](){
        std::string cmd = cmdLine->text().toStdString();
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




