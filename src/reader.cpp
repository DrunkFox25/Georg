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
    else if(cmdargs[0] == "run"){
        D.addNoise(1e-25);
        double diff = D.update();

        Log << "Diff: " << diff << "\n";

        if(D.descend() != Eigen::Success) return 1;
        
        Log << "Change: " << D.x_vals << "\n";
    }
    else if(cmdargs[0] == "runn"){
        int n = stoi(cmdargs[1]);
        while(n--){
            D.addNoise(1e-25);
            double diff = D.update();

            Log << "Diff: " << diff << "\n";

            if(D.descend() != Eigen::Success) return 1;

            Log << "Change: " << D.x_vals << "\n";

            if(diff < 1e-15){
                Log << "diff small exiting early" << "\n";
                break;
            }
        }
    }
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

int Reader::createExpr(std::string exprtype, std::string polystr, expression &C){//maybe at some point don't expand functions, just leave them as is, like in polystack
    if(exprtype != "POLY"){Log << "fuck you; it's not a bug yet; so don't even try with them general expressions yet\n" << std::flush; return 1;}

    int numvars = namevars.size();

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

    typedef std::vector<polyword> simpleexpr;

    std::vector<simpleexpr> varstack;

    polystr += '\0';

    int out = 0;

    std::function<int(std::string::iterator&, const std::vector<int>&)> readpoly;
    readpoly = [&](std::string::iterator &it, const std::vector<int> &fstack){//sry I rly had no other viable choice
        simpleexpr curr;
        std::string currWord = "";
        bool gonnabefunc = false;
        std::string::iterator funcit;


        while(true){
            char c = *it++;

            if(!currWord.empty() && (isalpha(c) ? isdigit(currWord[0]) : !(c == '.' || isdigit(c)))){
                //Log << "word: " << currWord << "\n" << std::flush;
                if(isalpha(currWord[0])){
                    if(varnames.count(currWord)) curr.push_back(polyword(2, varnames[currWord]));
                    else if(parsedFuncs.count(currWord)){gonnabefunc = true; funcit = parsedFuncs[currWord].begin();}
                    else{Log << "dumbass can't even remeber thier own variable names\n" << std::flush; out = 2;}
                }
                else if(currWord[0] == '$'){
                    int index = stoi(currWord.substr(1))-1;
                    if(index == -1){Log << "blud doesn't know what not 0 indexed means, from here the read is corrupted\n" << std::flush; out = 6; index = 0;};
                    curr.push_back(polyword(3, fstack[index]));
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
                    std::vector<int> funcstack;
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
    };

    if(out != 0) return out;

    auto it = polystr.begin();
    std::vector<int> fstack;
    readpoly(it, fstack);

    if(it != polystr.end()){Log << "fuckass doesn't know how to use parethisys. how tf do you spell that tho actually?\n"; return 8;}

    std::vector<int>& varsused = C.opnums;

    for(auto &expr : varstack){//if you uncomment these you get a clean af log of varstack
        //Log << "{";
        for(auto &word : expr){

            //Log << (int)word.type << ": ";
            //if(word.type == 0) Log << word.val;
            //else if(word.type == 1) Log << word.c;
            //else Log << word.varindex;
            //Log << ", ";

            if(word.type == 2) varsused.push_back(word.varindex);
        }
        //Log << "},\n" << std::flush;
    }

    sort(varsused.begin(), varsused.end());
    varsused.erase(unique(varsused.begin(), varsused.end()), varsused.end());

    int numvarsused = varsused.size();

    std::vector<int> invOpNum(numvars, -1);
    for(int j = 0; j < numvarsused; j++) invOpNum[varsused[j]] = j;

    for(auto &expr : varstack){
        for(auto &word : expr){
            if(word.type == 2) word.varindex = invOpNum[word.varindex];
        }
        expr.push_back(polyword(-1, '\0'));
    }

    std::vector<orderedPoly<cplx>> polystack(varstack.size(), orderedPoly<cplx>(numvarsused));

    for(int j = 0; j < varstack.size(); j++){
        std::vector<polyword> &expr = varstack[j];

        int k = 0;
        while(k < expr.size()-1){
            cplx coeff = 1;
            std::vector<int> stackindex = {};
            std::vector<int> purevars(numvarsused, 0);
            
            for(; expr[k].type == 1; k++){
                if(expr[k].c == '-') coeff *= -1;
            }
            while(k < expr.size()-1){
                if(expr[k].type == 0){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        coeff *= pow(expr[k].val, expr[k+2].val);
                        k += 2;
                    }
                    else coeff *= expr[k].val;
                }
                else if(expr[k].type == 2){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        purevars[expr[k].varindex] += (int)expr[k+2].val.real();
                        k += 2;
                    }
                    else purevars[expr[k].varindex]++;
                }
                else if(expr[k].type == 3){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        int cnt = expr[k+2].val.real();
                        while(cnt--) stackindex.push_back(expr[k].varstackindex);
                        k += 2;
                    }
                    else stackindex.push_back(expr[k].varstackindex);
                }
                else if(expr[k].type == 1){
                    if(expr[k].c == '-' || expr[k].c == '+') break;
                    if(expr[k].c == '^'){Log << "bruh\n" << std::flush; return 5;}
                    if(expr[k].c == '*'){}
                    if(expr[k].c == '/'){Log << "divison by error, if you couldn't tell divison is not implemented yet you bafoon\n" << std::flush; return 3;}
                }
                else{
                    Log << "this branch of the if/else is impossible to reach, congratz\n" << std::flush;
                    return 7;
                }
                k++;
            }

            orderedPoly<cplx> out(numvarsused);
            out.add_term(coeff, purevars);
            for(auto &i : stackindex) out = out*polystack[i];
            polystack[j] += out;
        }
        polystack[j].compress();
    }

    C.numvars = C.opnums.size();
    C.P = polystack[polystack.size()-1].P;

    Log << "expr successfully read\n" << std::flush;
    
    return 0;
}
