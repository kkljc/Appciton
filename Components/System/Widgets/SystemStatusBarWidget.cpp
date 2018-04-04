#include <QPixmap>
#include <QPainter>
#include "SystemStatusBarWidget.h"
#include "FontManager.h"
#include "PatientManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include "NetworkManager.h"
#include "PublicMenuManager.h"
#include "WiFiProfileMenu.h"
#include "WindowManager.h"

SystemStatusBarWidget *SystemStatusBarWidget::_selfObj = NULL;
#define ICON_WIDTH 32

/**************************************************************************************************
 * 隐藏图标。
 *************************************************************************************************/
void SystemStatusBarWidget::hideIcon(SystemIconLabel iconlabel)
{
    if (iconlabel >= SYSTEM_ICON_LABEL_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if(l)
    {
        l->setPixmap(_icons[SYSTEM_ICON_NONE]);
        _iconMap.value(iconlabel)->setFocusPolicy(Qt::NoFocus);
    }
}

/**************************************************************************************************
 * 函数说明:
 *         修改图标。
 * 参数说明:
 *         iconlabel:对应图标标签。
 *         status:具体图标。
 *         enableFocus:是否可以获取焦点。
 *************************************************************************************************/
void SystemStatusBarWidget::changeIcon(SystemIconLabel iconlabel, int status, bool enableFocus)
{
    if (iconlabel >= SYSTEM_ICON_LABEL_NR)
    {
        return;
    }

    if (status >= SYSTEM_ICON_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if(l)
    {
        l->setPixmap(_icons[(SystemStatusIcon)status]);
        IWidget *w = _iconMap.value(iconlabel);
        if (NULL != w)
        {
            if (enableFocus)
            {
                if (w->focusPolicy() == Qt::NoFocus)
                {
                    w->setFocusPolicy(Qt::StrongFocus);
                    windowManager.setFocusOrder();
                }
            }
            else
            {
                if (w->focusPolicy() == Qt::StrongFocus)
                {
                    w->setFocusPolicy(Qt::NoFocus);
                    windowManager.setFocusOrder();
                }
            }
        }
    }
}

/**************************************************************************************************
 * 函数说明:
 *         获取焦点子控件。
 *************************************************************************************************/
void SystemStatusBarWidget::getSubFocusWidget(QList<QWidget*> &subWidgets) const
{
    subWidgets.clear();

    for(int i = 0; i < SYSTEM_ICON_LABEL_NR; i++)
    {
        IWidget *subWidget = _iconMap.value((SystemIconLabel)i);
        if (NULL != subWidget)
        {
            subWidgets.append(subWidget);
        }
    }
}

/**************************************************************************************************
 * 图标点击事件。
 *************************************************************************************************/
void SystemStatusBarWidget::onIconClicked(int iconLabel)
{
    if(iconLabel == SYSTEM_ICON_LABEL_WIFI)
    {
        QRect r = windowManager.getMenuArea();
        int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
        int y = r.y() + (r.height() - publicMenuManager.height());

        publicMenuManager.popup(&wifiProfileMenu,x,y);
    }
}

/**************************************************************************************************
 * 绘画事件。
 *************************************************************************************************/
void SystemStatusBarWidget::paintEvent(QPaintEvent */*e*/)
{
    //do nothing
}

/***************************************************************************************************
 * timerEvent : use to update wifi icon
 **************************************************************************************************/
void SystemStatusBarWidget::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == _timer.timerId())
    {
        if(networkManager.isWifiWorking())
        {
            changeIcon(SYSTEM_ICON_LABEL_WIFI, networkManager.isWiFiConnected()
                       ? SYSTEM_ICON_WIFI_CONNECTED
                       :SYSTEM_ICON_WIFI_DISCONNECTED, true);
        }
        else
        {
            changeIcon(SYSTEM_ICON_LABEL_WIFI, SYSTEM_ICON_NONE);
        }
    }
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void SystemStatusBarWidget::showEvent(QShowEvent */*e*/)
{
    //pacer icon
    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(SYSTEM_ICON_LABEL_PACER)->children().at(1));
    if(l)
    {
        if(patientManager.getPacermaker())
        {
            l->setPixmap(_icons[SYSTEM_ICON_PACER_ON]);
        }
        else
        {
            l->setPixmap(_icons[SYSTEM_ICON_PACER_OFF]);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemStatusBarWidget::SystemStatusBarWidget() : IWidget("SystemStatusBarWidget"),
    _signalMapper(new QSignalMapper(this))
{
    _icons[SYSTEM_ICON_NONE] = QPixmap();
    _icons[SYSTEM_ICON_USB_CONNECTED] = QPixmap("/usr/local/nPM/icons/usb.png");
    _icons[SYSTEM_ICON_WIFI_CONNECTED] = QPixmap("/usr/local/nPM/icons/wifi.png");
    _icons[SYSTEM_ICON_WIFI_DISCONNECTED] = QPixmap("/usr/local/nPM/icons/wifi_disconnected.png");
    _icons[SYSTEM_ICON_PACER_OFF] = QPixmap("/usr/local/nPM/icons/PaceMarkerOff.png");
    _icons[SYSTEM_ICON_PACER_ON] = QPixmap("");//"/usr/local/nPM/icons/Pacemarkeron.png");打开时不需要图标。

    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);

    for(int i = 0; i < SYSTEM_ICON_LABEL_NR; i++)
    {
        IWidget *w = new IWidget(QString("icon%1").arg(i+1));
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setFocusPolicy(Qt::NoFocus);
        w->setFixedWidth(ICON_WIDTH);
        connect(w, SIGNAL(released()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(w, i);
        QHBoxLayout *layout = new QHBoxLayout(w);
        layout->setContentsMargins(0,0,0,0);
        QLabel *l = new QLabel();
        l->setPixmap(_icons[SYSTEM_ICON_NONE]);
        layout->addWidget(l, 0, Qt::AlignCenter);
        mainLayout->addWidget(w);
        _iconMap.insert((SystemIconLabel)i, w);
    }
    mainLayout->addStretch();
    connect(_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(iconClicked(int)));
    connect(this, SIGNAL(iconClicked(int)), this, SLOT(onIconClicked(int)));

    _timer.start(1000, this);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemStatusBarWidget::~SystemStatusBarWidget()
{
   _timer.stop();
}