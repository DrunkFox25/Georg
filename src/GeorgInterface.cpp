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

Interface::drawcmd::drawcmd(std::string Type, QColor Color, int Width, int Style) : color(Color), width(Width), style(Style){set(Type);}

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

void Interface::GeorgCanvas::paint(QPainter painter){//update this to draw multipule at once using the respective Qt functions; add gpu support for impilicit

    //https://doc.qt.io/qt-6/qpainter.html#drawArc-2
    //https://doc.qt.io/qt-6/qpainter.html#drawText-3
    //https://doc.qt.io/qt-6/qpainter.html#drawLine-4
    //https://doc.qt.io/qt-6/qpainter.html#drawPoint-2
    //https://doc.qt.io/qt-6/qpainter.html#drawEllipse-4

    //todo: put line labels where it intersects the veiwbox
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    QBrush brush;
    painter.setBrush(brush);


    for(drawcmd &cmd : drawer){
        bool newpen = false;
        if(cmd.color.isValid()){pen.setColor(cmd.color); newpen = true;}
        if(cmd.width != -1){pen.setWidth(cmd.width); newpen = true;}
        if(cmd.style != -1){pen.setStyle((Qt::PenStyle)cmd.style); newpen = true;}
        if(newpen) painter.setPen(pen);

        std::vector<double> vals;
        for(expression &C : cmd.vars) vals.push_back(C.eval(S->vars).real());//gonna check later if cplx -> then not draw it, but for now

        std::string label = cmd.labelstr;
        if(label.empty()) label = std::to_string(cmd.labelexp.eval(S->vars).real());

        //gotta adjust for veiwbox

        if(cmd.type == drawcmd::DRAW_SET){}
        else if(cmd.type == drawcmd::DRAW_POINT){
            painter.drawPoint(vals[0], vals[1]);
        }
        else if(cmd.type == drawcmd::DRAW_LINE){
            //extend out to the edges of the veiwbox
            //temporaryly just a segment
            if(vals.size() == 4) painter.drawLine(vals[0], vals[1], vals[2], vals[3]);
            if(vals.size() == 2) painter.drawLine(-10000, vals[1]-10000*vals[0], 10000, vals[1]+10000*vals[0]);//vals[0]x+vals[1] = y
            else Log << "draw line with invalid number of variables\n" << std::flush;
        }
        else if(cmd.type == drawcmd::DRAW_SEGMENT){
            //permanently just a segment
            painter.drawLine(vals[0], vals[1], vals[2], vals[3]);
        }
        else if(cmd.type == drawcmd::DRAW_RAY){
            //temporaryly just a segment
            painter.drawLine(vals[0], vals[1], vals[2], vals[3]);
        }
        else if(cmd.type == drawcmd::DRAW_CIRCLE){
            painter.drawEllipse(vals[0]-vals[2], vals[1]-vals[2], vals[2]*2, vals[2]*2);
        }
        else if(cmd.type == drawcmd::DRAW_ELLIPSE){
            painter.drawEllipse(vals[0]-vals[2], vals[1]-vals[3], vals[2]*2, vals[3]*2);
        }
        else if(cmd.type == drawcmd::DRAW_ARC){
            painter.drawArc(vals[0]-vals[2], vals[1]-vals[3], vals[2]*2, vals[3]*2, vals[4]*16, (vals[5]-vals[4])*16);
        }
        else if(cmd.type == drawcmd::DRAW_ANGLE){
            //this has 6 variables
            double angle1 = 180*std::atan2(vals[1]-vals[3], vals[0]-vals[2])/std::numbers::pi;
            double angle2 = 180*std::atan2(vals[5]-vals[3], vals[4]-vals[2])/std::numbers::pi;
            angle2 -= angle1;
            if(std::abs(angle2-90) < 1e-9){
                painter.save();
                painter.translate(vals[2], vals[3]);
                painter.rotate(angle1);
                painter.drawLine(0, 10, 10, 10);
                painter.drawLine(10, 10, 10, 0);
                painter.restore();
            }
            else painter.drawArc(vals[2]-10, vals[3]-10, 20, 20, angle1*16, angle2*16);
        }
        else if(cmd.type == drawcmd::DRAW_IMPLICIT){
            Log << "Implicit drawing not supported yet\n" << std::flush;
        }

        painter.drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
    }
    return;
}

void Interface::GeorgCanvasWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    return this->paint(QPainter(this));
}