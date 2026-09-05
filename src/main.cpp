#include "main.hpp"

using namespace std;








struct QTextEditStream : public std::streambuf, public std::ostream {
    QPlainTextEdit *m_textEdit;
    std::string m_buffer;

    QTextEditStream() : std::ostream(this), m_textEdit(nullptr) {}

    QTextEditStream(QPlainTextEdit *textEdit) : std::ostream(this), m_textEdit(textEdit){}

    virtual std::streambuf::int_type overflow(std::streambuf::int_type v) override {
        if (v == std::streambuf::traits_type::eof()) return std::streambuf::traits_type::not_eof(v);

        char c = std::streambuf::traits_type::to_char_type(v);
        m_buffer.push_back(c);

        return v;
    }

    virtual int sync() override {
        flushToWidget();
        return 0;
    }

    void flushToWidget() {
        if (!m_buffer.empty()){
            m_textEdit->moveCursor(QTextCursor::End);
            m_textEdit->insertPlainText(QString::fromStdString(m_buffer));
            m_buffer.clear();
        }
    }
};

int main(int argc, char *argv[]){//todo move all code to src
    QApplication app(argc, argv);

    QWidget mainWindow;
    mainWindow.setWindowTitle("Georg");
    mainWindow.resize(1200, 700);

    QGridLayout *layout = new QGridLayout(&mainWindow);
    
    QLabel *titleLabel = new QLabel(&mainWindow);
    QPlainTextEdit *LogDisplay = new QPlainTextEdit(&mainWindow);
    QPlainTextEdit *stateIn = new QPlainTextEdit(&mainWindow);
    QPushButton *regenButton = new QPushButton(&mainWindow);
    QLineEdit *cmdLine = new QLineEdit(&mainWindow);
    Interface::GeorgCanvasWidget *canvas = new Interface::GeorgCanvasWidget(&mainWindow);

    regenButton->setText("Regen State");

    titleLabel->setText("Georg - Geogebra 2");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    // Span argument: (widget, startRow, startColumn, rowSpan, columnSpan)
    layout->addWidget(titleLabel, 0, 0, 1, 10);
    layout->addWidget(LogDisplay, 1, 5, 9, 5);
    layout->addWidget(stateIn, 1, 0, 9, 5);
    layout->addWidget(regenButton, 10, 0, 1, 3);
    layout->addWidget(cmdLine, 10, 3, 1, 7);
    layout->addWidget(canvas, 11, 0, 5, 5);

    //ofstream Logf("Log.txt");
    LogDisplay->setReadOnly(true);
    //LogDisplay->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    QTextEditStream QLog(LogDisplay);
    TeeStream qlog(QLog, cout);
    log_def = &qlog;

    string fname = "tests/test6.json";//"assets/regenstateex.txt"
    ifstream regenStateEx(fname);
    if (!regenStateEx.is_open()) Log << "Error: Could not open the file: " << fname << "\n" << endl;
    stringstream buffer;
    buffer << regenStateEx.rdbuf();
    stateIn->insertPlainText(QString::fromStdString(buffer.str()));
    regenStateEx.close();

    Reader R;
    Interface::dispList display;
    decentEngine D;

    QObject::connect(regenButton, &QPushButton::clicked, [&](){
        regenButton->setText("Loading New State");
        vector<string> cmds;
        R.regenState(stateIn->toPlainText().toStdString(), cmds, canvas->drawer, display, D);
        for(auto &cmd : cmds) R.readcmd(cmd, D);
        regenButton->setText("Regen State");
    });

    QObject::connect(cmdLine, &QLineEdit::returnPressed, [&](){
        string cmd = cmdLine->text().toStdString();
        cmdLine->setText("Processing");
        R.readcmd(cmd, D);
        cmdLine->clear();
    });

    mainWindow.show();

    return app.exec();
}





//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{if you uncomment this it is a dope af rainbow}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}

