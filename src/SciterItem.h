#ifndef SCITERITEM_H
#define SCITERITEM_H

#include <QQuickItem>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>

class SciterRenderer;

class SciterItem : public QQuickItem
{
    Q_OBJECT
public:
    SciterItem(QQuickItem *parent = nullptr);

signals:
    void hoverEnterEventReceived(QHoverEvent *event);
    void hoverMoveEventReceived(QHoverEvent *event);
    void hoverLeaveEventReceived(QHoverEvent *event);

public slots:
    void sync();
    void cleanup();


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    SciterRenderer *m_renderer;

    // QQuickItem interface
protected:
    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
};

#endif // SCITERITEM_H
