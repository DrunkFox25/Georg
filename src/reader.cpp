#include "reader.hpp"

int Reader::readcmd(std::string cmd, decentEngine &D){//acts on decent engine
    std::vector<std::string> cmdargs;
    std::string curr;

    for(auto &c : cmd){
        if(isspace(c)){
            if(!curr.empty()) cmdargs.push_back(curr);
            curr.clear();
        }
        else curr += c;
    }
    if(!curr.empty()) cmdargs.push_back(curr);

    if(cmdargs[0] == "regen") D.regen();
    else if(cmdargs[0] == "query"){
        Log << "Vars: " << D.vars << "\n";
    }
    else if(cmdargs[0] == "modify"){
        int var;
        if(isalpha(cmdargs[1][0])) var = varnames[cmdargs[1]];
        else var = stoi(cmdargs[1]);
        D.vars[var] = stoc(cmdargs[2]);
    }
    Log << std::flush;
    return 0;
}


int Reader::regenState(std::string state, std::vector<std::string> &cmdsout, Interface::drawList &drawer, Interface::dispList &display, State &S){//does not touch D, only S
    S.clear();
    drawer.clear();
    display.clear();
    drawer.push_back(Interface::drawcmd("SET", Qt::blue, 4, Qt::SolidLine));
    //todo: make all tests work
    //make diff a default variable

    //add some way to display diff

    QByteArray rawJson = QByteArray::fromStdString(state);
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawJson, &parseError);
    Log << "JSON Parsed with ";
    if(parseError.error != QJsonParseError::NoError){Log << "error: " << parseError.errorString().toStdString() << "\n" << std::flush; return 1;}
    Log << "no error\n " << std::flush;
    if(!jsonDoc.isObject()){Log << "erm aktushally nevermind that shit your json is not a object you braindead pigfucker\n" << std::flush; return 1;}

    QJsonObject rootObj = jsonDoc.object();

    std::string testname = rootObj["name"].toString().toStdString();

    Log << "loading " << testname << "\n" << std::flush;

    QJsonObject vars = rootObj["vars"].toObject();

    S.rsNumvars(vars.size());

    const QStringList varnamesJson = vars.keys();
    for(int i = 0; i < varnamesJson.size(); i++){
        const QString &var = varnamesJson[i];
        std::string stdvar = var.toStdString();
        varnames[stdvar] = i;
        namevars.push_back(stdvar);

        QJsonArray varshitidkfucknamingshit = vars[var].toArray();
        QJsonValue val = varshitidkfucknamingshit[0];

        if(val.isString()) S.vars[i] = stoc(val.toString().toStdString());
        else S.vars[i] = val.toDouble();

        for(QJsonValue value : varshitidkfucknamingshit){
            if(value.isString() && value.toString() == "fixed") S.fixed[i] = true;
            if(value.isBool() && value.toBool()) S.fixed[i] = true;
        }
    }

    QJsonObject funcs = rootObj["func"].toObject();

    for(const QString &funcname : funcs.keys()) parsedFuncs[funcname.toStdString()] = funcs[funcname].toString().toStdString()+'\0';

    QJsonArray constrs = rootObj["constr"].toArray();

    for(int i = 0; i < constrs.size(); i++){
        expression C;
        int out = createExpr(constrs[i].toObject()["type"].toString().toStdString(), constrs[i].toObject()["expression"].toString().toStdString(), C);
        if(out != 0) return out;
        S.add(C);
    }

    QJsonArray draws = rootObj["draws"].toArray();

    for(QJsonValue val : draws) drawer.push_back(Interface::drawcmd(val.toObject(), *this));

    QJsonValue valdisp = rootObj["valuedisplay"];
    if(valdisp.isString() && valdisp.toString().toStdString() == "auto"){
        std::vector<int> displvars(namevars.size());
        for(int i = 0; i < namevars.size(); i++) displvars[i] = i;

        display = {Interface::dispcmd(namevars, displvars, std::vector<bool>(namevars.size(), true))};
    }
    else{
        for(QJsonValue val : valdisp.toArray()) display.push_back(Interface::dispcmd(val.toObject(), *this));
    }

    QJsonArray cmds = rootObj["cmds"].toArray();

    for(QJsonValue val : cmds) cmdsout.push_back(val.toString().toStdString());

    return 0;
}

