#include "GeorgInterface.hpp"





void Interface::drawcmd::set(std::string Type){
    if(Type == "SET") type = DRAW_SET;
    else if(Type == "POINT") type = DRAW_POINT;
    else if(Type == "LINE") type = DRAW_LINE;
    else if(Type == "SEGMENT") type = DRAW_SEGMENT;
    else if(Type == "RAY") type = DRAW_RAY;
    else if(Type == "CIRCLE") type = DRAW_CIRCLE;
    else if(Type == "ELLIPSE") type = DRAW_ELLIPSE;
    else if(Type == "ARC") type = DRAW_ARC;
    else if(Type == "ANGLE") type = DRAW_ANGLE;
    else if(Type == "IMPLICIT") type = DRAW_IMPLICIT;

    return;
}

Interface::drawcmd::drawcmd(){}

Interface::drawcmd::drawcmd(QJsonObject ob, Reader &R){
    set(ob["type"].toString().toStdString());

    if(ob.contains("color")) color.fromString(ob["color"].toString());
    if(ob.contains("width")) width = ob["width"].toDouble();
    if(ob.contains("style")){
        std::string st = ob["style"].toString().toStdString();
        if(st == "nopen") style = 0;
        if(st == "solid") style = 1;
        if(st == "dash") style = 2;
        if(st == "dot") style = 3;
        if(st == "dashdot") style = 4;
        if(st == "dashdotdot") style = 5;
        if(st == "custom") style = 6;//not done yet
    }
    if(ob.contains("label")){
        if(ob["label"].isString()) labelstr = ob["label"].toString().toStdString();
        else{
            expression C;
            int out = R.createExpr(ob["label"].toObject()["type"].toString().toStdString(), ob["label"].toObject()["expression"].toString().toStdString(), C);
            //if(out != 0) return out;
            labelexp = C;
        }
    }

    if(type == DRAW_SET) return;

    QJsonArray vlist = ob["varlist"].toArray();
    for(auto v : vlist){
        if(v.isDouble()) vars.push_back(expression(v.toDouble()));
        if(v.isString()){
            std::string vstr = v.toString().toStdString();
            if(R.varnames.count(vstr) > 0){
                expression C;
                C.numvars = 1;
                C.P.push_back({1, {1}});
                C.opnums = {R.varnames[vstr]};
                vars.push_back(C);
            }
            else{
                vars.push_back(expression(stoc(vstr)));
            }
        }
        else{
            expression C;
            int out = R.createExpr(v.toObject()["type"].toString().toStdString(), v.toObject()["expression"].toString().toStdString(), C);
            //if(out != 0) return out;
            vars.push_back(C);
        }
    }
}

void Interface::dispcmd::set(std::string Type){
    if(Type == "VLIST") type = DISP_VALUE_LIST;
    else if(Type == "SLIDER") type = DISP_SLIDER;
    else if(Type == "COMPLEX") type = DISP_COMPLEX;

    return;
}

Interface::dispcmd::dispcmd(){}

Interface::dispcmd::dispcmd(std::string Type){set(Type);}

Interface::dispcmd::dispcmd(std::vector<std::string> Name, std::vector<int> Vars, std::vector<bool> Modify) : sz(Vars.size()), name(std::move(Name)), vars(std::move(Vars)), modify(std::move(Modify)){}

Interface::dispcmd::dispcmd(QJsonObject ob, Reader &R){
    set(ob["type"].toString().toStdString());

    QJsonArray varlist = ob["varlist"].toArray();
    QJsonArray names;
    if(!ob.contains("names")) names = varlist;
    else names = ob["names"].toArray();

    for(auto x : varlist) vars.push_back(R.varnames[x.toString().toStdString()]);
    for(auto x : names) name.push_back(x.toString().toStdString());

    sz = vars.size();

    if(ob.contains("modif")){
        QJsonValue modif = ob["modif"];
        if(modif.isBool()) modify.resize(sz, modif.toBool());
        else{
            for(auto x : modif.toArray()) modify.push_back(x.toBool());
        }
    }
    else modify.resize(sz, true);

    if(type == DISP_VALUE_LIST){}
    else if(type == DISP_SLIDER){
        if(ob["range"].isArray()){
            for(auto r : ob["range"].toArray()) ranges.push_back({
                r.toObject()["from"].toDouble(),
                r.toObject()["to"].toDouble()
            });
        }
        else ranges.resize(sz, {
            ob["range"].toObject()["from"].toDouble(),
            ob["range"].toObject()["to"].toDouble()
        });
    }
    else if(type == DISP_COMPLEX){
        auto r = ob["range"].toObject();
        crange.left = r["from"].toDouble();
        crange.right = r["to"].toDouble();
        crange.down = r["fromi"].toDouble();
        crange.up = r["toi"].toDouble();
    }
}

void Interface::GeorgCanvas::paint(QPainter painter){
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    QBrush brush;

    pen.setColor(Qt::blue);
    pen.setWidth(4);
    pen.setStyle(Qt::SolidLine);

    painter.setPen(pen);
    painter.setBrush(brush);

    for(drawcmd &cmd : drawer){
        if(cmd.color.isValid()) pen.setColor(cmd.color);
        if(cmd.width != -1) pen.setWidth(cmd.width);
        if(cmd.style != -1) pen.setStyle((Qt::PenStyle)cmd.style);
    }

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

void Interface::GeorgCanvasWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    return this->paint(QPainter(this));
}