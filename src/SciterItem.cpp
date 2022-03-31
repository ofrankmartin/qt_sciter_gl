#include "SciterItem.h"

#include <QQuickWindow>
#include <QThread>
#include <QOpenGLContext>

#include "SciterRenderer.h"

SciterItem::SciterItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &SciterItem::handleWindowChanged);
}

void SciterItem::sync()
{
    bool needInitialize = false;
    if (!m_renderer)
    {
        m_renderer = new SciterRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &SciterRenderer::paint, Qt::DirectConnection);
        connect(m_renderer, &SciterRenderer::update, this, &QQuickItem::update, Qt::QueuedConnection);

        // Input events
        connect(this, &SciterItem::hoverEnterEventReceived, m_renderer, &SciterRenderer::hoverEnterEvent, Qt::QueuedConnection);
        connect(this, &SciterItem::hoverMoveEventReceived , m_renderer, &SciterRenderer::hoverMoveEvent , Qt::QueuedConnection);
        connect(this, &SciterItem::hoverLeaveEventReceived, m_renderer, &SciterRenderer::hoverLeaveEvent, Qt::QueuedConnection);

        needInitialize = true;
    }

    QSize intSize(this->size().width(), this->size().height());
    m_renderer->setViewportSize(intSize * window()->devicePixelRatio());
    m_renderer->setWindow(window());

    if (needInitialize)
    {
        m_renderer->init();
    }
}

void SciterItem::cleanup()
{
    if (m_renderer)
    {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void SciterItem::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &SciterItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &SciterItem::cleanup, Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);

        connect(window(), &QQuickWindow::afterSynchronizing, this, [](){
            qDebug() << "### afterSynchronizing";
        });
        connect(window(), &QQuickWindow::afterRendering, this, [](){
            qDebug() << "afterRendering";
        });

        connect(window(), &QQuickWindow::sceneGraphInitialized, this, [win]() {
            qDebug() << "sceneGraphInitialized";
            win->update();
        });
    }
}

void SciterItem::hoverEnterEvent(QHoverEvent *event)
{
    emit hoverEnterEventReceived(event);
}

void SciterItem::hoverMoveEvent(QHoverEvent *event)
{
    emit hoverMoveEventReceived(event);
}

void SciterItem::hoverLeaveEvent(QHoverEvent *event)
{
    emit hoverLeaveEventReceived(event);
}

