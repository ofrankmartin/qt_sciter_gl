#include "SciterRenderer.h"

#include <string>
#include <locale>
#include <codecvt>

#include <QOpenGLContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSGRendererInterface>

#include <sciter-x.h>

#include "../facade-resources.cpp"

SciterRenderer::SciterRenderer(QObject *parent) : QObject(parent), m_window(nullptr), m_glInitialized(false), m_needDrawing(true)
{
    m_ticks.start();
}

SciterRenderer::~SciterRenderer()
{
    SciterProcX(this, SCITER_X_MSG_DESTROY());
}

void SciterRenderer::setNeedDrawing(bool needDrawing)
{
    if (m_needDrawing != needDrawing)
    {
        m_needDrawing = needDrawing;
        if (m_window)
        {
            emit this->update();
        }
    }
}

void SciterRenderer::paint()
{
    if (!m_glInitialized)
    {
        initializeOpenGLFunctions();
        m_glInitialized = true;
    }

    // SCITER RENDERING
    SciterProcX(this, SCITER_X_MSG_HEARTBIT(m_ticks.elapsed()));

    if (m_needDrawing)
    {
        this->setNeedDrawing(false);
        if (m_lastViewportSize != m_viewportSize)
        {
            SciterProcX(this, SCITER_X_MSG_SIZE(m_viewportSize.width(), m_viewportSize.height()));
            m_lastViewportSize = m_viewportSize;
        }

        glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        SciterProcX(this, SCITER_X_MSG_PAINT());

        m_window->resetOpenGLState();
    }
    // ------------------
}

bool SciterRenderer::init()
{

    auto version = SAPI()->version;
    qDebug().nospace() << "Version: 0x" << hex << version;

    // create an engine instance
    SBOOL ok = SciterProcX(this, SCITER_X_MSG_CREATE(GFX_LAYER_SKIA, FALSE));
    if (!ok) return false;

    SciterProcX(this, SCITER_X_MSG_RESOLUTION(96 * m_window->devicePixelRatio()));
    SciterProcX(this, SCITER_X_MSG_SIZE(m_viewportSize.width(), m_viewportSize.height()));

    // setup callbacks
    SciterSetOption(this, SCITER_SET_DEBUG_MODE, TRUE);
    //SciterSetCallback(this, handle_notification, this);
    SciterSetCallback(this, &SciterRenderer::handleNotification, this);

    if (!sciter::archive::instance().open(aux::elements_of(resources)))
    {
        qDebug() << "Unable to open resources.";
    }
    else
    {
        auto *fileToLoad = WSTR("this://app/main.htm");
        QString strFileToLoad = QString::fromStdU16String(std::u16string(fileToLoad));
        ok = SciterLoadFile(this, fileToLoad);
        if (!ok)
        {
            qDebug() << "No" << strFileToLoad;
        }
    }
    return true;
}

void SciterRenderer::hoverEnterEvent(QHoverEvent *event)
{
    MOUSE_EVENTS me = MOUSE_ENTER;
    POINT pos = { event->pos().x(), event->pos().y() };
    SciterProcX(this, SCITER_X_MSG_MOUSE(me, MOUSE_BUTTONS(0), KEYBOARD_STATES(0), pos));
    qDebug() << "hoverEnterEvent" << event->pos();
}

void SciterRenderer::hoverMoveEvent(QHoverEvent *event)
{
    MOUSE_EVENTS me = MOUSE_MOVE;
    POINT pos = { event->pos().x(), event->pos().y() };
    SciterProcX(this, SCITER_X_MSG_MOUSE(me, MOUSE_BUTTONS(0), KEYBOARD_STATES(0), pos));
    //qDebug() << "hoverMoveEvent" << event->pos();
}

void SciterRenderer::hoverLeaveEvent(QHoverEvent *event)
{
    MOUSE_EVENTS me = MOUSE_LEAVE;
    POINT pos = { event->pos().x(), event->pos().y() };
    SciterProcX(this, SCITER_X_MSG_MOUSE(me, MOUSE_BUTTONS(0), KEYBOARD_STATES(0), pos));
    qDebug() << "hoverLeaveEvent" << event->pos();
}

UINT SciterRenderer::onLoadData(LPSCN_LOAD_DATA pnmld)
{
    // your custom loader is here

    // LPCBYTE pb = 0; UINT cb = 0;
    aux::wchars wu = aux::chars_of(pnmld->uri);

    if (wu.like(WSTR("this://app/*")))
    {
        // try to get them from archive first
        aux::bytes adata = sciter::archive::instance().get(wu.start + 11);
        if (adata.length)
            ::SciterDataReady(pnmld->hwnd, pnmld->uri, adata.start, adata.length);
    }
    return LOAD_OK;
}

UINT SciterRenderer::onDataLoaded(LPSCN_DATA_LOADED pnm)
{
    QString dataLoaded = QString::fromStdU16String(std::u16string((char16_t*)pnm->uri));
    qDebug() << dataLoaded << "loaded";
    return 0;
}

UINT SciterRenderer::attachBehavior(LPSCN_ATTACH_BEHAVIOR lpab)
{
    // attach native behaviors (if we have any)
    sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
    if (pb)
    {
        lpab->elementTag  = pb;
        lpab->elementProc = sciter::event_handler::element_proc;
        return 1;
    }
    return 0;
}

UINT SciterRenderer::onInvalidateRect(LPSCN_INVALIDATE_RECT pnm)
{
    (void)pnm;
    this->setNeedDrawing(true);
    return 0;
}

// notifiaction cracker
UINT SciterRenderer::handleNotification(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID callbackParam)
{
    // Crack and call appropriate method
    // here are all notifiactions
    SciterRenderer *renderer = static_cast<SciterRenderer*>(callbackParam);
    switch (pnm->code) {
    case SC_LOAD_DATA: return renderer->onLoadData((LPSCN_LOAD_DATA)pnm);
    case SC_DATA_LOADED: return renderer->onDataLoaded((LPSCN_DATA_LOADED)pnm);
    case SC_ATTACH_BEHAVIOR: return renderer->attachBehavior((LPSCN_ATTACH_BEHAVIOR)pnm);
    case SC_INVALIDATE_RECT: return renderer->onInvalidateRect((LPSCN_INVALIDATE_RECT)pnm);
    case SC_ENGINE_DESTROYED: break;
    }
    return 0;
}
