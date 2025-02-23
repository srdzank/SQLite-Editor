#pragma once
#include <QWidget>
#include <QPainter>

class CVideo  : public QWidget
{
	Q_OBJECT

public:
	CVideo(QWidget *parent);
	~CVideo();
protected:
	void paintEvent(QPaintEvent* event) override;
};
