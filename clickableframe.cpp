#include "clickableframe.h"
#include <QPainter>
#include <QPainterPath>

ClickableFrame::ClickableFrame(QWidget *parent)
    : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
}

void ClickableFrame::enterEvent(QEnterEvent *event)
{
    hovered = true;
    update();
    QFrame::enterEvent(event);
}

void ClickableFrame::leaveEvent(QEvent *event)
{
    hovered = false;
    update();
    QFrame::leaveEvent(event);
}

void ClickableFrame::setDrawTopBar(bool enabled)
{
    drawTopBar = enabled;
    update(); // repaint the frame
}

void ClickableFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Clip everything to rounded corners
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12); // match your stylesheet radius
    p.setClipPath(path);

    // Draw top bar if enabled
    if (drawTopBar)
    {
        QColor barColor(20, 20, 24, 200);
        int barHeight = 100;
        p.fillRect(QRect(0, 0, width(), barHeight), barColor);
    }

    // Hover overlay
    if (hovered)
    {
        QColor hoverColor(255, 255, 255, 25);
        p.fillRect(rect(), hoverColor);
    }

}

