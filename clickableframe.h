#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>
#include <QMouseEvent>

class ClickableFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ClickableFrame(QWidget *parent = nullptr);

    void setDrawTopBar(bool enabled);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton)
            emit clicked();
        QFrame::mousePressEvent(event);
    }
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;


private:
    bool drawTopBar = true;
    bool hovered = false;

};

#endif // CLICKABLEFRAME_H
