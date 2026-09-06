#include "GeorgInterface.hpp"



Interface::QTextEditStream::QTextEditStream() : std::ostream(this), m_textEdit(nullptr) {}

Interface::QTextEditStream::QTextEditStream(QPlainTextEdit *textEdit) : std::ostream(this), m_textEdit(textEdit){}

std::streambuf::int_type Interface::QTextEditStream::overflow(std::streambuf::int_type v){
    if (v == std::streambuf::traits_type::eof()) return std::streambuf::traits_type::not_eof(v);

    char c = std::streambuf::traits_type::to_char_type(v);
    m_buffer.push_back(c);

    return v;
}

int Interface::QTextEditStream::sync(){
    flushToWidget();
    return 0;
}

void Interface::QTextEditStream::flushToWidget() {
    if (!m_buffer.empty()){
        m_textEdit->moveCursor(QTextCursor::End);
        m_textEdit->insertPlainText(QString::fromStdString(m_buffer));
        m_buffer.clear();
    }
    return;
}













void Interface::clear(){
    m_scene->clear();
}

Interface::canvas::canvas(QWidget *parent){
    setRenderHints(QPainter::Antialiasing);
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-200, -200, 400, 400);
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);//this is because idon't know
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setScene(m_scene);
}

class gLabel{//General Label Class -> update ts to tex support
private:
    expression labelexp;
    std::string labelstr;
public:
    void getLabel(State* S, std::string &label){
        if(!labelstr.empty()) label = labelstr;
        if(!labelexp.empty()) label = std::to_string(labelexp.eval(S->vars).real());
        else label = "";
        return;
    }

    void set(QJsonValue v, Reader &R){
        if(v.isString()) cmd.labelstr = v.toString().toStdString();
        else if(v.isObject) R.createExpr(v.toObject()["type"].toString().toStdString(), v.toObject()["expression"].toString().toStdString(), cmd.labelexp);
        return;
    }
}

struct baseDrawCmd : public gLabel, public QGraphicalItem{//to use this override bounding box, paint
    State* S;
    gLabel label;
    int numvars;
    std::vector<expression> vars;
    QColor color = QColor();//invalid by default
    int width = -1;
    int style = -1;//Qt::PenStyle would use the enum but no invalid value

    std::vector<cplx> vals;//is assumed to be write size
    std::string currlabel;

    void eval(QPainter* painter){//outputs vals for you
        painter->setRenderHint(QPainter::Antialiasing);

        bool newpen = false;
        Qpen pen = painter->pen();
        if(cmd.color.isValid()){pen.setColor(cmd.color); newpen = true;}
        if(cmd.width != -1){pen.setWidth(cmd.width); newpen = true;}
        if(cmd.style != -1){pen.setStyle((Qt::PenStyle)cmd.style); newpen = true;}
        if(newpen) painter.setPen(pen);

        for(int i = 0; i < numvars; i++) vals[i] = vars[i].eval(S->vars);

        label.getLabel(S, currlabel);

        return;
    }
}


/*
https://doc.qt.io/qt-6/qpainter.html#drawArc-2
https://doc.qt.io/qt-6/qpainter.html#drawText-3
https://doc.qt.io/qt-6/qpainter.html#drawLine-4
https://doc.qt.io/qt-6/qpainter.html#drawPoint-2
https://doc.qt.io/qt-6/qpainter.html#drawEllipse-4
*/


#define bigassnumber = 2000;

struct noDrawCmd : public baseDrawCmd{
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {return;}
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct pointDrawCmd : public baseDrawCmd{
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawLine(-bigassnumber*(vals[2]-vals[0])+vals[0], -bigassnumber*(vals[3]-vals[1])+vals[1], bigassnumber*(vals[2]-vals[0])+vals[0], bigassnumber*(vals[3]-vals[1])+vals[1]);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct lineDrawCmd : public baseDrawCmd{//temporaryly just a segment //extend out to the edges of the veiwbox, but label there //todo: put line's label where it intersects the veiwbox
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawLine(-bigassnumber*(vals[2]-vals[0])+vals[0], -bigassnumber*(vals[3]-vals[1])+vals[1], bigassnumber*(vals[2]-vals[0])+vals[0], bigassnumber*(vals[3]-vals[1])+vals[1]);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct segmentDrawCmd : public baseDrawCmd{//permanently just a segment
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawLine(vals[0], vals[1], vals[2], vals[3]);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct rayDrawCmd : public baseDrawCmd{//temporaryly just a segment //todo: put ray's label where it intersects the veiwbox
    //later can be 3 vars
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawLine(vals[0], vals[1], vals[0]+bigassnumber*(vals[2]-vals[0]), vals[1]+bigassnumber*(vals[3]-vals[1]));
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct circleDrawCmd : public baseDrawCmd{
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawEllipse(vals[0]-vals[2], vals[1]-vals[2], vals[2]*2, vals[2]*2);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct lipsDrawCmd : public baseDrawCmd{////this isn't a true ellipse yet, it can't be rotated yet
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawEllipse(vals[0]-vals[2], vals[1]-vals[3], vals[2]*2, vals[3]*2);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct arcDrawCmd : public baseDrawCmd{
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawArc(vals[0]-vals[2], vals[1]-vals[3], vals[2]*2, vals[3]*2, vals[4]*16, (vals[5]-vals[4])*16);
        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct angleDrawCmd : public baseDrawCmd{
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        double angle1 = 180*std::atan2(vals[1]-vals[3], vals[0]-vals[2])/std::numbers::pi;
        double angle2 = 180*std::atan2(vals[5]-vals[3], vals[4]-vals[2])/std::numbers::pi;
        angle2 -= angle1;
        if(std::abs(angle2-90) < 1e-9){
            painter->save();
            painter->translate(vals[2], vals[3]);
            painter->rotate(angle1);
            painter->drawLine(0, 10, 10, 10);
            painter->drawLine(10, 10, 10, 0);
            painter->restore();
        }
        else painter->drawArc(vals[2]-10, vals[3]-10, 20, 20, angle1*16, angle2*16);

        painter->drawText(vals[0], vals[1], QString::fromStdString(label));//probably should be color independent
        return;
    }
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporary
};

struct implicitDrawCmd : public baseDrawCmd{
    //exetended_expresion im;//allows for extra vars like x,y used in implicit
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {}
    QRectF boundingRect() const override {return this->scene()->sceneRect();}//temporarly temporary
};





int Interface::canvas::add(QJsonObject ob, Reader &R, State &S){
    baseDrawCmd cmd;

    cmd.S = &S;

    int out = 0;

    std::string Type = ob["type"].toString().toStdString();

    const std::vector<std::string> styles = {
        "nopen",
        "solid",
        "dash",
        "dot",
        "dashdot",
        "dashdotdot",
        "custom"
    }

    if(ob.contains("color")) cmd.color.fromString(ob["color"].toString());
    if(ob.contains("width")) cmd.width = ob["width"].toDouble();
    if(ob.contains("style")) cmd.style = std::find(styles.begin(), styles.end(), ob["style"].toString().toStdString())-styles.begin();
    if(ob.contains("label")) cmd.label.set(ob["label"]);
    if(ob.contains("vlist")){
        QJsonArray vlist = ob["vlist"].toArray();
        cmd.numvars = vlist.size();
        for(auto v : vlist){
            if(v.isDouble()) vars.push_back(expression(v.toDouble()));
            if(v.isString()){
                std::string vstr = v.toString().toStdString();
                if(std::isalpha(vstr[0])) cmd.vars.push_back(expression(1, R.varnames[vstr]));
                else cmd.vars.push_back(expression(stoc(vstr)));
            }
            else if(v.isObject(){
                expression C;
                int out = R.createExpr(v.toObject()["type"].toString().toStdString(), v.toObject()["expression"].toString().toStdString(), C);
                if(out != 0) return out;
                vars.push_back(C);
            }
            else{
                return 1;
            }
        }
    }

    if(Type == "SET"){
        m_scene->addItem((noCmdDraw)&cmd);
    }
    else if(Type == "POINT"){
        if(cmd.numvars != 2) return 2;
        m_scene->addItem((pointCmdDraw)&cmd);
    }
    else if(Type == "LINE"){
        if(cmd.numvars != 4) return 2;
        m_scene->addItem((lineCmdDraw)&cmd);
    }
    else if(Type == "SEGMENT"){
        if(cmd.numvars != 4) return 2;
        m_scene->addItem((segmentCmdDraw)&cmd);
    }
    else if(Type == "RAY"){
        if(cmd.numvars != 4) return 2;
        m_scene->addItem((rayCmdDraw)&cmd);
    }
    else if(Type == "CIRCLE"){
        if(cmd.numvars != 3) return 2;
        m_scene->addItem((circleCmdDraw)&cmd);
    }
    else if(Type == "ELLIPSE"){
        if(cmd.numvars != 4) return 2;
        m_scene->addItem((lipsCmdDraw)&cmd);
    }
    else if(Type == "ARC"){
        if(cmd.numvars != 6) return 2;
        m_scene->addItem((arcCmdDraw)&cmd);
    }
    else if(Type == "ANGLE"){
        if(cmd.numvars != 6) return 2;
        m_scene->addItem((angleCmdDraw)&cmd);
    }
    else if(Type == "IMPLICIT"){
        QJsonArray ar = ob["implicitf"].toArray();
        std::string xvar = ar[0].toString().toStdString();
        std::string yvar = ar[1].toString().toStdString();
        std::string implicitfunction = ar[2].toString().toStdString();
        Log << "Implicit drawing not supported yet\n" << std::flush;
        //m_scene->addItem((implicitCmdDraw)&cmd);
    }
    else{
        Log << "Invalid Draw Command\n" << std::flush;
        return 3;
    }

    return 0;
}









void Interface::display::clear(){//erradicates all descedants
    if(container) delete container;
    if(layout) delete layout;
    container = new QWidget(this);
    layout = new VBoxLayout(container);
    setWidgetResizable(true);
    setWidget(container);
    return;
}

Interface::display::display(QWidget *parent){
    QScrollArea(parent);
    clear();
}

void Interface::display::add(QJsonObject ob, Reader &R){
    int sz = 0;
    std::vector<std::string> name;
    std::vector<int> vars;
    std::vector<bool> modify;
    std::vector<std::pair<double, double>> ranges;
    rect<double> crange;

    std::string type = ob["type"].toString().toStdString();

    QJsonArray varlist = ob["vlist"].toArray();
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

    if(type == "VLIST"){
        for(int i = 0; i < disp.sz; i++){
            QLineEdit *valueoflist = new QLineEdit(container);
            valueoflist->setText(QString::fromStdString(disp.name[i]));
            layout->addWidget(valueoflist);
            valueoflist->setReadOnly(!disp.modify[i]);
            if(disp.modify[i]){
                QObject::connect(valueoflist, &QLineEdit::returnPressed, [&](){D.vars[disp.vars[i]] = stoc(valueoflist->text().toStdString());});
            }
        }
    }
    else if(type == "SLIDER"){
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

        for(int i = 0; i < disp.sz; i++){
            QSlider *slider = new QSlider(Qt::Horizontal, container);
            slider->setMinimum(disp.ranges[i].first);
            slider->setMaximum(disp.ranges[i].second);
            slider->setValue(D.vars[disp.vars[i]].real());
            layout->addWidget(slider);
            slider->setEnabled(disp.modify[i]);
            if(disp.modify[i]){
                QObject::connect(slider, &QSlider::valueChanged, [&](){D.vars[disp.vars[i]] = cplx(slider->value(), D.vars[disp.vars[i]].imag());});
            }
        }
    }
    else if(type == "COMPLEX"){
        auto r = ob["range"].toObject();
        crange.left = r["from"].toDouble();
        crange.right = r["to"].toDouble();
        crange.down = r["fromi"].toDouble();
        crange.up = r["toi"].toDouble();

        Interface::complexPlaneWidget *complexPlane = new Interface::complexPlaneWidget(container);
        for(int i = 0; i < disp.sz; i++) complexPlane->addComplexNumber(new Interface::complexPlaneWidget::cPoint(&D, disp.vars[i], 5, disp.modify[i]));
        layout->addWidget(complexPlane);
    }
}






struct complexPlaneWidget : public QGraphicsView{
    struct cPoint : public QGraphicsItem{
        State *S;
        std::string label;//need to implement drawing this
        int index;
        bool isMoveable;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        QRectF boundingRect() const override;

        QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    };

    QGraphicsScene *m_scene;

    complexPlaneWidget(QWidget *parent = nullptr);

    void complexPlaneWidget::addPoint(State *S, int index, bool isMovable, std::string label);
};













void complexPlaneWidget::cPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPen pen(QColor("#0c4904"), 8, Qt::DashLine);
    pen.setCosmetic(true);

    int x = S->vars[index].real();
    int y = -S->vars[index].imag();
    bool small = false;

    if(x > 5){x = 5; small = true;}
    if(x < -5){x = -5; small = true;}
    if(y > 5){y = 5; small = true;}
    if(y < -5){y = -5; small = true;}

    if(small) pen.setWidth(6);

    painter->setPen(pen);
    painter->drawPoint(x, y);

    return;
}

QRectF complexPlaneWidget::boundingRect() const {
    double diam = 8;

    int x = S->vars[index].real();
    int y = -S->vars[index].imag();
    bool small = false;

    if(x > 5){x = 5; small = true;}
    if(x < -5){x = -5; small = true;}
    if(y > 5){y = 5; small = true;}
    if(y < -5){y = -5; small = true;}

    if(small) diam = 6;

    diam /= transform().m11();

    return QRectF(x-diam/2, y-diam/2, diam, diam);
}

QVariant complexPlaneWidget::cPoint::itemChange(GraphicsItemChange change, const QVariant &value){
    if(change == QGraphicsItem::ItemSelectedChange){
        //draw current complex value somewhere
    }
    else if(isMoveable && change == QGraphicsItem::ItemPositionChange && scene()){
        QPointF newPos = value.toPointF();
        
        S->vars[index] = cplx(newPos.x(), -newPos.y());
    }
    return QGraphicsItem::itemChange(change, value);
}

complexPlaneWidget::complexPlaneWidget(QWidget *parent) : QGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);

    setMinimumSize(200, 200);
    setMaximumSize(200, 200);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);

    QPen axisPen(QColor("#7f8c8d"), 1.5, Qt::DashLine);//todo: add labels to the axes and ticks and numbers

    m_scene->setSceneRect(-100, -100, 200, 200);
    m_scene->addLine(-100, 0, 100, 0, axisPen);
    m_scene->addLine(0, -100, 0, 100, axisPen);

    scale(20, 20);//make this based on crange
}

void complexPlaneWidget::addPoint(State *S, int index, bool isMovable, std::string label){
    cPoint *point = new cPoint();
    point->S = S;
    point->index = index;
    point->isMoveable = isMoveable;
    point->label = label;
    m_scene->addItem(point);
}



