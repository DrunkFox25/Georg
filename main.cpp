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
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
//#include <QRhiWidget>//for later when I will integrate with custom frag shaders
//#include <rhi/qrhi.h>

using namespace std;

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





/*
{
    "vars" : {
        "varname" : [
            "fixed",/nothing
            value
        ],
    },
    "func" : {
        "funcname": "expression using $0, $1, $2, ..." -> 0 indexed, not 1 indexed
    },
    "constr" : [
        {
            "type" : "general"/"poly"
            "expression" : ".."
        },...
    ],
    "draw" : [
        {
            "type" : "",
            "varlist" : [""],
            "color" : hex reader idk,
            "thickness" :
        },...
    ],
    "cmds" : [
        "strings"
    ]
}
*/
struct drawcmd{
    drawmcmd(){}
    drawcmd(QJsonObject obj){}
};

struct Interface{
    ostream& log;
    State S;
    decentEngine D{S};

    map<string, int> varnames;
    vector<string> namevars;

    vector<drawcmd> drawer;

    Interface(ostream& Log) : log(Log){}


    int readcmd(string cmd){
        vector<string> cmdargs = views::split(cmd, ' ') | ranges::to<vector<string>>();

        if(cmdargs[0] == "regen") D.regen();
        else if(cmdargs[0] == "run"){
            D.addNoise(1e-25);
            double diff = D.update();

            log << "Diff: " << diff << "\n";

            if(D.descend() != Eigen::Success) return 1;
            
            log << "Change: " << D.x_vals << "\n";
        }
        else if(cmdargs[0] == "runn"){
            int n = stoi(cmdargs[1]);
            while(n--){
                D.addNoise(1e-25);
                double diff = D.update();

                log << "Diff: " << diff << "\n";

                if(D.descend() != Eigen::Success) return 1;

                log << "Change: " << D.x_vals << "\n";

                if(diff < 1e-15){
                    log << "diff small exiting early" << "\n";
                    break;
                }
            }
        }
        else if(cmdargs[0] == "query"){
            log << "Vars: " << S.vars << "\n";
            log << "constrVals: " << S.constrVal << "\n";
        }
        else if(cmdargs[0] == "modify"){
            int var;
            if(isalpha(cmdargs[1][0])) var = varnames[cmdargs[1]];
            else var = stoi(cmdargs[1]);
            S.vars[var] = stoc(cmdargs[2]);
        }
        return 0;
    }


    int regenState(string state){
        QByteArray rawJson = QByteArray::fromStdString(state);
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(rawJson, &parseError);
        log << "JSON Parsed with ";
        if(parseError.error != QJsonParseError::NoError){log << "error: " << parseError.errorString().toStdString() << "\n" << flush; return 1;}
        log << "no error\n " << flush;
        if(!jsonDoc.isObject()){log << "erm aktushally nevermind that shit your json is not a object you braindead pigfucker\n" << flush; return 1;}

        QJsonObject rootObj = jsonDoc.object();

        QJsonObject vars = rootObj["vars"].toObject();
        QJsonObject funcs = rootObj["func"].toObject();
        QJsonArray constrs = rootObj["constr"].toArray();
        QJsonArray draw = rootObj["draw"].toArray();
        QJsonArray cmds = rootObj["draw"].toArray();

        S.rsN(constrs.size());
        S.rsNumvars(vars.size());

        for(int i = 0; i < S.numvars; i++){
            QString &var = vars.keys()[i];
            string stdvar = var.toStdString();
            varnames[stdvar] = i;
            namevars.push_back(stdvar);

            QJsonArray varshitidkfucknamingshit = vars["var"].toArray();
            QJsonValue val = varshitidkfucknamingshit[0];

            if(val.isString()) S.vars[i] = stoc(val.toString().toStdString());
            else S.vars[i] = val.toDouble();

            for(QJsonValue value : varshitidkfucknamingshit){
                if(value.isString() && value.toString() == "fixed") S.fixed[i] = true;
                if(value.isBool() && value.toBool()) S.fixed[i] = true;
            }
        }

        map<string, string> parsedFuncs;
        for(const QString &funcname : funcs.keys()) parsedFuncs[funcname.toStdString()] = funcs[funcname].toString().toStdString();

        //maybe at some point don't expand functions, just leave them as is, just replace variables with $1,$2,$3,...

        for(int i = 0; i < S.n; i++){
            //do parentheses matching first
            QJsonObject cnstr = constrs[i].toObject();
            if(cnstr["type"] != "POLY") log << "fuck you; it's not a bug yet; so don't even try with them general expressions yet\n" << flush;
            string poly = cnstr["expression"].toString().toStdString();
            constr &C = S.constrs[i];
            vector<int> &opnums = S.constrOpNums[i];
            vector<int> match(poly.size(), -1);

            struct polyword{
                char type;
                union{
                    char c;
                    int varindex;
                    int varstackindex;
                    cplx val;
                };

                polyword(char t, cplx v) : type(t), val(v){}
                polyword(char t, char C) : type(t), c(C){}
                polyword(char t, int v) : type(t), varindex(v){}
            };

            typedef vector<polyword> simpleexpr;

            vector<simpleexpr> varstack;//every expression get's put here, to reference the ith here you use !i
            //if a simple expression starts with somthing with a .first of 4, then it is the shell of an evaluated function

            std::function<int(std::string::iterator&, const std::vector<int>&)> readpoly;

            readpoly = [&](string::iterator &it, const vector<int> &fstack){//sry I rly had no other viable choice
                simpleexpr curr;
                string currWord = "";
                bool gonnabefunc = false;
                string::iterator funcit;

                while(true){
                    char c = *it++;

                    if(!isalnum(c)){
                        if(isalpha(currWord[0])){
                            if(varnames.count(currWord)) curr.push_back(polyword(2, varnames[currWord]));
                            else if(parsedFuncs.count(currentWord)){gonnabefunc = true; funcit = parsedFuncs[currWord].begin();}
                            else{log << "dumbass can't even remeber thier own variable names\n" << flush; return 2;}
                        }
                        if(currWord[0] == '$'){
                            curr.push_back(polyword(3, fstack[stoi(currWord.substr(1))]));
                        }
                        else{
                            curr.push_back(polyword(0, stoc(currWord)));
                        }
                        currWord.clear();
                    }

                    if(isspace(c)){}
                    else if(isop(c)) curr.push_back(polyword(1, c));
                    else if(c == '('){
                        if(gonnabefunc){
                            vector<int> funcstack;
                            while(*(it-1) != ')') funcstack.push_back(readpoly(it, fstack));
                            curr.push_back(polyword(3, readpoly(funcit, funcstack)));
                        }
                        else{
                            curr.push_back(polyword(3, readpoly(it, fstack)));
                        }
                    }
                    else if(c == ')' || c == ',' || c == '\0') break;
                    else currWord += c;
                }

                varstack.push_back(curr);
                return varstack.size()-1;
            }

            auto it = poly.beign();
            vector<int> fstack();
            readpoly(it, fstack);

            if(it != poly.end()){log << "fuckass doesn't know how to use parethisys. how tf do you spell that tho actually?\n" << flush; return 2;}

            for(auto &expr : varstack){
                for(auto &[type, val] : expr){
                    if(type == 2) S.constrOpNums[i].push_back(val);
                }
            }

            sort(S.constrOpNums[i].begin(), S.constrOpNums[i].end());
		    S.constrOpNums[i].erase(unique(S.constrOpNums[i].begin(), S.constrOpNums[i].end()), S.constrOpNums[i].end());

            S.constrDir[i].resize(S.constrOpNums[i].size());
            S.constrs[i].numvars = S.constrOpNums[i].size();

            vector<int> invOpNum(S.numvars, -1);
            for(int j = 0; j < S.constrOpNums[i].size(); j++) invOpNum[S.constrOpNums[i][j]] = j;

            for(auto &expr : varstack){
                for(auto &[type, val] : expr){
                    if(type == 2) val = invOpNum[val];
                }
                expr.push_back(-1, '\0');
            }

            vector<orderedPoly<cplx>> polystack(varstack.size(), orderedPoly<cplx>(S.constrOpNums[i].size()));//coeff is after terms as this way sorting helps us

            for(int j = 0; j < varstack.size(); j++){
                vector<polyword> &expr = varstack[j];

                int k = 0;
                while(k < expr.size()-1){
                    cplx coeff = 1;
                    vector<int> stackindex = {};
                    vector<int> purevars(S.constrOpNums[i].size(), 0);
                    
                    for(; expr[k].type == 1; k++){
                        if(expr[k].c == '-') coeff *= -1;
                    }
                    while(k < expr.size()-1){
                        if(expr[k].type == 0){
                            if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                                if(expr[k+2].type != 0){log << "dfsfgjfhgnbfdghfgbv\n" << flush; return 4;}
                                coeff *= pow(expr[k].val, expr[k+2].val);
                                k += 2;
                            }
                            else ceoff *= expr[k].val;
                        }
                        else if(expr[k].type == 2){
                            if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                                if(expr[k+2].type != 0){log << "dfsfgjfhgnbfdghfgbv\n" << flush; return 4;}
                                purevars[expr[k].varindex] += (int)expr[k+2].val.real();
                                k += 2;
                            }
                            else purevars[expr[k].varindex]++;
                        }
                        else if(expr[k].type == 3){
                            if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                                if(expr[k+2].type != 0){log << "dfsfgjfhgnbfdghfgbv\n" << flush; return 4;}
                                int cnt = expr[k+2].val.real();
                                while(cnt--) stackindex.push_back(expr[k].varstackindex);
                                k += 2;
                            }
                            else stackindex.push_back(expr[k].varstackindex);
                        }
                        else if(expr[k].type == 1){
                            if(expr[k].c == '-' || expr[k].c == '+') break;
                            if(expr[k].c == '^'){log << "bruh\n" << flush; return 3;}
                            if(expr[k].c == '*'){}
                            if(expr[k].c == '/'){log << "divison by error, if you couldn't tell divison is not implemented yet you bafoon!\n" << flush; return 3;}    
                        }
                        else{
                            log << "this branch of the if/else is impossible to reach, congratz\n" << flush;
                            return 5;
                        }
                        k++;
                    }

                    orderedPoly<cplx> out(S.constrOpNums[i].size());
                    out.add_term(coeff, purevals);
                    for(auto &i : stackindex) out = out*polystack[i];
                    polystack[j] += out;
                }
                polystack[j].compress();
            }
            S.constrs[i] = reinterpret_cast<poly<cplx>>(polystack[polystack.size()-1]);
        }

        for(QJsonValue &val : draws) drawer.push_back(drawcmd(val.toObject()));

        for(QJsonValue &val : cmds){
            int out = readcmd(val.toString().toStdString());
            if(out){
                log << "exiting setup, bad cmd\n" << flush;
                return out+16;
            }
        }

        return 0;
    }
};

//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{if you uncomment this it is a dope af rainbow}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}






struct QTextEditStream : public std::streambuf, public std::ostream {
    QPlainTextEdit *m_textEdit;
    std::string m_buffer;

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

struct TeeStream : public std::ostream {
    struct TeeBuf : public std::streambuf {
        TeeBuf(std::streambuf* buf1, std::streambuf* buf2) : m_buf1(buf1), m_buf2(buf2) {}

        virtual int_type overflow(int_type c) override {
            if (c == traits_type::eof()) {
                return traits_type::not_eof(c);
            }
            
            bool ok1 = m_buf1->sputc(c) != traits_type::eof();
            bool ok2 = m_buf2->sputc(c) != traits_type::eof();

            return (ok1 && ok2) ? c : traits_type::eof();
        }

        virtual int sync() override {
            int res1 = m_buf1->pubsync();
            int res2 = m_buf2->pubsync();
            return (res1 == 0 && res2 == 0) ? 0 : -1;
        }

        std::streambuf* m_buf1;
        std::streambuf* m_buf2;
    };

    TeeBuf m_tbuf;

    TeeStream(std::ostream& stream1, std::ostream& stream2) : std::ostream(&m_tbuf), m_tbuf(stream1.rdbuf(), stream2.rdbuf()) {}
};

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    QWidget mainWindow;
    mainWindow.setWindowTitle("Georg");
    mainWindow.resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);
    
    QLabel *titleLabel = new QLabel(&mainWindow);
    QPlainTextEdit *logDisplay = new QPlainTextEdit(&mainWindow);
    QPlainTextEdit *stateIn = new QPlainTextEdit(&mainWindow);
    QPushButton *regenButton = new QPushButton(&mainWindow);
    QLineEdit *cmdLine = new QLineEdit(&mainWindow);
    GeorgCanvas *canvas = new GeorgCanvas(&mainWindow);

    regenButton->setText("Regen State");
    canvas->setMinimumSize(300, 200);

    titleLabel->setText("Georg - Geogebra 2");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    layout->addWidget(titleLabel);
    layout->addWidget(logDisplay);
    layout->addWidget(stateIn);
    layout->addWidget(regenButton);
    layout->addWidget(cmdLine);
    layout->addWidget(canvas);

    //ofstream logf("log.txt");
    logDisplay->setReadOnly(true);
    //logDisplay->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    QTextEditStream Qlog(logDisplay);
    TeeStream logs(Qlog, cout);

    Interface inter(logs);

    QObject::connect(regenButton, &QPushButton::clicked, [&](){
        regenButton->setText("Loading New State");
        inter.regenState(logDisplay->toPlainText().toStdString());
        regenButton->setText("Regen State");
    });

    QObject::connect(cmdLine, &QLineEdit::returnPressed, [&](){
        string cmd = cmdLine->text().toStdString();
        cmdLine->setText("Processing");
        inter.readcmd(cmd);
        cmdLine->clear();
    });

    mainWindow.show();

    return app.exec();
}

