#include "QVideo.h"

CVideo::CVideo(QWidget *parent)
	: QWidget(parent)
{
}

CVideo::~CVideo()
{
}

void CVideo::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    // Custom painting code goes here
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Hello, World!");
}