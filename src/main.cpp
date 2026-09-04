#include "main.hpp"
#include "GeorgInterface.hpp"
#include "reader.hpp"








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
    mainWindow.resize(1000, 700);

    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
    
    QLabel *titleLabel = new QLabel(&mainWindow);
    QPlainTextEdit *LogDisplay = new QPlainTextEdit(&mainWindow);
    QPlainTextEdit *stateIn = new QPlainTextEdit(&mainWindow);
    QPushButton *regenButton = new QPushButton(&mainWindow);
    QLineEdit *cmdLine = new QLineEdit(&mainWindow);
    Interface::GeorgCanvas *canvas = new Interface::GeorgCanvas(&mainWindow);

    regenButton->setText("Regen State");
    canvas->setMinimumSize(300, 200);

    titleLabel->setText("Georg - Geogebra 2");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    layout->addWidget(titleLabel);
    layout->addWidget(LogDisplay);
    layout->addWidget(stateIn);
    layout->addWidget(regenButton);
    layout->addWidget(cmdLine);
    layout->addWidget(canvas);

    //ofstream Logf("Log.txt");
    LogDisplay->setReadOnly(true);
    //LogDisplay->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    QTextEditStream QLog(LogDisplay);
    TeeStream qlog(QLog, cout);
    log_def = &qlog;

    string fname = "tests/test5.json";//"assets/regenstateex.txt"
    ifstream regenStateEx(fname);
    if (!regenStateEx.is_open()) Log << "Error: Could not open the file: " << fname << "\n" << endl;
    stringstream buffer;
    buffer << regenStateEx.rdbuf();
    stateIn->insertPlainText(QString::fromStdString(buffer.str()));
    regenStateEx.close();

    Reader R;
    vector<Reader::drawcmd> drawer;
    vector<Reader::disp> display;
    decentEngine D;

    QObject::connect(regenButton, &QPushButton::clicked, [&](){
        regenButton->setText("Loading New State");
        vector<string> cmds;
        R.regenState(stateIn->toPlainText().toStdString(), cmds, drawer, display, D);
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

