#ifndef GEORG_SRC_GEORGINTERFACE_HPP_
#define GEORG_SRC_GEORGINTERFACE_HPP_

#include "main.hpp"

//labels for lines should be at the end of the screen

struct Reader;

namespace Interface{//add tex
    struct QTextEditStream : public std::streambuf, public std::ostream {
        QPlainTextEdit *m_textEdit;
        std::string m_buffer;

        QTextEditStream();

        QTextEditStream(QPlainTextEdit *textEdit);

        virtual std::streambuf::int_type overflow(std::streambuf::int_type v) override;

        virtual int sync() override;

        void flushToWidget();
    };

    struct canvas : public QGraphicsView{
        State* S;

        QGraphicsScene *m_scene = nullptr;

        void clear();

        canvas(QWidget *parent = nullptr);

        int add(QJsonObject ob, Reader &R, State &S);//update to make this more safe, move this to reader meathod
    };

    struct display : public QScrollArea{
        QWidget *container = nullptr;
        QVBoxLayout *layout = nullptr;

        void clear();

        display(QWidget *Parent = nullptr);

        void add(QJsonObject ob, Reader &R);//move this to reader meathod
    };
}

#endif // GEORG_SRC_GEORGINTERFACE_HPP_