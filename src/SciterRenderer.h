#ifndef SCITERRENDERER_H
#define SCITERRENDERER_H

#include <QOpenGLFunctions>
#include <QSize>
#include <QElapsedTimer>


#include <sciter-x.h>

class QOpenGLShaderProgram;
class QQuickWindow;
class QHoverEvent;

class SciterRenderer :
        public QObject,
        protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit SciterRenderer(QObject *parent = nullptr);
    ~SciterRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }
    void setNeedDrawing(bool needDrawing);

    UINT onLoadData(LPSCN_LOAD_DATA pnmld);
    UINT onDataLoaded(LPSCN_DATA_LOADED pnm);
    UINT attachBehavior(LPSCN_ATTACH_BEHAVIOR lpab);
    UINT onInvalidateRect(LPSCN_INVALIDATE_RECT pnm);

public slots:
    void paint();
    bool init();

    // Input Events
    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);

signals:
    void update();

private:
    QSize m_viewportSize;
    QSize m_lastViewportSize;
    QQuickWindow *m_window;

    QElapsedTimer m_ticks;
    bool m_glInitialized;
    bool m_needDrawing;

    static UINT handleNotification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam);
};

#endif // SCITERRENDERER_H
