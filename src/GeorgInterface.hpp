#pragma once

#include "main.hpp"

//labels for lines should be at the end of the screen

struct Reader;

namespace Interface{//add tex
    struct drawcmd{
        enum {
            DRAW_NONE,
            DRAW_SET,
            DRAW_POINT,
            DRAW_LINE,
            DRAW_SEGMENT,
            DRAW_RAY,
            DRAW_CIRCLE,
            DRAW_ELLIPSE,
            DRAW_ARC,
            DRAW_ANGLE,
            DRAW_IMPLICIT
        } type = DRAW_NONE;
        std::vector<expression> vars;
        QColor color = QColor();//invalid by default
        int width = -1;
        int style = -1;//Qt::PenStyle would use the enum but no invalid value
        expression labelexp;
        std::string labelstr;

        void set(std::string Type);

        drawcmd();

        drawcmd(std::string Type, QColor Color = QColor(), int Width = -1, int Style = -1);

        drawcmd(QJsonObject ob, Reader &R);//update to make this more safe
    };

    struct dispcmd{
        enum {
            DISP_VALUE_LIST,
            DISP_SLIDER,
            DISP_COMPLEX
        } type = DISP_VALUE_LIST;
        int sz = 0;
        std::vector<std::string> name;
        std::vector<int> vars;
        std::vector<bool> modify;
        std::vector<std::pair<double, double>> ranges;
        rect<double> crange;

        void set(std::string Type);

        dispcmd();

        dispcmd(std::string Type);

        dispcmd(std::vector<std::string> Name, std::vector<int> Vars, std::vector<bool> Modify);

        dispcmd(QJsonObject ob, Reader &R);//update to make this more safe
    };

    typedef std::vector<drawcmd> drawList;
    typedef std::vector<dispcmd> dispList;

    struct GeorgCanvas{
        drawList drawer;
        State* S;

        void paint(QPainter painter);
    };

    struct GeorgCanvasWidget : public QWidget, public GeorgCanvas{
        using QWidget::QWidget;

        void paintEvent(QPaintEvent *event) override;
    };
}

