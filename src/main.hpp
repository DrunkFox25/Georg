#pragma once

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
#include <QRect>

#include <QRhiWidget>//for later when I will integrate with custom frag shaders
//#include <rhi/qrhi.h>

#undef emit//qt keyword collision with std emit


#include "GeorgEngine.hpp"
#include "GeorgInterface.hpp"
#include "reader.hpp"

using namespace std;