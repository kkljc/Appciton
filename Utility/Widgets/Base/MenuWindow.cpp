/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#include "MenuWindow.h"
#include "FontManager.h"
#include <QBoxLayout>
#include <QStackedWidget>
#include "MenuSidebar.h"
#include "MenuSidebarItem.h"
#include "WindowManager.h"
#include "Button.h"
#include "LanguageManager.h"
#include <QList>
#include <QPainter>

class MenuWindowPrivate
{
public:
    MenuWindowPrivate()
        : sidebar(NULL),
          stackWidget(NULL),
          retBtn(NULL),
          rightLayout(NULL),
          menuPath(""),
          isBelongToLeftWidget(false)
    {
    }

    MenuSidebar *sidebar;
    QStackedWidget *stackWidget;
    Button *retBtn;
    QBoxLayout *rightLayout;
    QString menuPath;
    bool isBelongToLeftWidget;  // 聚焦点否属于左边菜单
};

MenuWindow::MenuWindow()
    : Window(), d_ptr(new MenuWindowPrivate())
{
    setWindowTitle("Menu Window");
    resize(800, 580);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setWindowLayout(layout);

    d_ptr->sidebar = new MenuSidebar();
    d_ptr->sidebar->setFixedWidth(200);
    layout->addWidget(d_ptr->sidebar);

    d_ptr->rightLayout = new QVBoxLayout();
    d_ptr->rightLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->rightLayout->setSpacing(0);
    layout->addLayout(d_ptr->rightLayout);

    d_ptr->stackWidget = new QStackedWidget();
    d_ptr->rightLayout->addWidget(d_ptr->stackWidget, 1);

    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    d_ptr->rightLayout->addWidget(line);

    connect(d_ptr->sidebar, SIGNAL(selectItemChanged(int)), this, SLOT(onSelectItemChanged(int)));
}

MenuWindow::~MenuWindow()
{
}

void MenuWindow::addMenuContent(MenuContent *menu)
{
    d_ptr->sidebar->addItem(menu->name());
    menu->doLayout();

    ScrollArea *scrollArea = new ScrollArea;
    scrollArea->setWidget(menu);
    scrollArea->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->stackWidget->addWidget(scrollArea);

    if (d_ptr->stackWidget->count() == 1)
    {
        // set the first Item
        d_ptr->sidebar->setChecked(menu->name());
    }
}

void MenuWindow::popup(const QString &menuName, const QVariant &param)
{
    d_ptr->sidebar->setChecked(menuName);

    // find current menucontent and set the show param
    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            content->setShowParam(param);
        }
    }

    windowManager.showWindow(this, WindowManager::ShowBehaviorCloseOthers);
}

void MenuWindow::setWindowTitlePrefix(const QString &prefix)
{
    QString strPath = prefix;
    QStringList pathList = strPath.split('-');

    d_ptr->menuPath.clear();
    if (pathList.count() < 2)
    {
        return;
    }
    d_ptr->menuPath  = trs(pathList.at(0));
    d_ptr->menuPath += "-";
    d_ptr->menuPath += trs(pathList.at(1));

    update();
}

bool MenuWindow::focusNextPrevChild(bool next)
{
    /*
     *  when the focus switch back to the menusidebar, we need to make menusidebar
     *  focus on the checked item.
     */

    QWidget *cur = focusWidget();
    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    QWidget *w = NULL;

    // 判断当前聚焦菜单是属于左侧还是右侧
    if (d_ptr->sidebar->isAncestorOf(cur))
    {
        d_ptr->isBelongToLeftWidget = true;
    }
    else if (area->isAncestorOf(cur) ||
             cur == d_ptr->retBtn)
    {
        d_ptr->isBelongToLeftWidget = false;
    }

    // 判断当前聚焦在关闭按钮的位置是否属于左侧聚焦区域
    bool isFocusCloseWidgetBelongToLeft = false;
    if (cur != d_ptr->retBtn &&
        !d_ptr->sidebar->isAncestorOf(cur) &&
        !area->widget()->isAncestorOf(cur))
    {
        if (d_ptr->isBelongToLeftWidget)
        {
            isFocusCloseWidgetBelongToLeft = true;
        }
    }

    if (isFocusCloseWidgetBelongToLeft)
    {
        int count = d_ptr->sidebar->itemCount();
        int newIndex = 0;
        if (!next)
        {
            newIndex =  count - 1;
        }
        MenuSidebarItem *item = d_ptr->sidebar->itemAt(newIndex);
        item->setFocus();

        return true;
    }

    if (d_ptr->isBelongToLeftWidget)
    {
        // keep the focus inside the menu sidebar
        int index = d_ptr->sidebar->indexOf(qobject_cast<MenuSidebarItem *>(cur));
        int count = d_ptr->sidebar->itemCount();

        if (index == 0 && !next)
        {
            w = cur->previousInFocusChain();
            // find previous focus widget in the focus chain
            while (w && cur != w)
            {
                if (w->isEnabled()
                        && (w->focusPolicy() & Qt::TabFocus)
                        && w->isVisibleTo(this)
                        && w->isEnabled()
                        && !d_ptr->sidebar->isAncestorOf(w)
                        && !area->isAncestorOf(w)
                        && d_ptr->retBtn != w)
                {
                    break;
                }
                w = w->previousInFocusChain();
            }
            w->setFocus();

            return true;
        }
        else if (index == count - 1 && next)
        {
            w = cur->nextInFocusChain();
            // find next focus widget in the focus chain
            while (w && cur != w)
            {
                if (w->isEnabled()
                        && (w->focusPolicy() & Qt::TabFocus)
                        && w->isVisibleTo(this)
                        && w->isEnabled()
                        && !d_ptr->sidebar->isAncestorOf(w)
                        && !area->isAncestorOf(w)
                        && d_ptr->retBtn != w)
                {
                    break;
                }
                w = w->nextInFocusChain();
            }
            w->setFocus();

            return true;
        }

        if (index >= 0)
        {
            int newIndex = 0;
            if (next)
            {
                newIndex = (index + 1) % count;
            }
            else
            {
                newIndex = (index + count -  1) % count;
            }
            MenuSidebarItem *item = d_ptr->sidebar->itemAt(newIndex);
            item->setFocus();
        }

        return true;
    }

    if (next)
    {
        w = cur->nextInFocusChain();
        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && w->isEnabled()
                    && !d_ptr->sidebar->isAncestorOf(w))
            {
                break;
            }
            w = w->nextInFocusChain();
        }

        if (w)
        {
            w->setFocus(Qt::TabFocusReason);
            if (area && area->isAncestorOf(w))
            {
                area->ensureWidgetVisible(w);
            }
            return true;
        }
    }
    else
    {
        w = cur->previousInFocusChain();

        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && w->isEnabled()
                    && !d_ptr->sidebar->isAncestorOf(w))
            {
                break;
            }
            w = w->previousInFocusChain();
        }

        if (w)
        {
            w->setFocus(Qt::BacktabFocusReason);
            if (area && area->isAncestorOf(w))
            {
                area->ensureWidgetVisible(w);
            }
            return true;
        }
    }

    return Window::focusNextPrevChild(next);
}

void MenuWindow::showEvent(QShowEvent *ev)
{
    /**
     * Here is a workround, we want the return button as the last widget to has focus in the window,
     * But the Qt's tab order is based on the construction order, so we make the return button create
     * at the last, after all the menu content is added
     */
    if (d_ptr->retBtn == NULL)
    {
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->setContentsMargins(4, 4, 8, 4);
        bottomLayout->addStretch(1);

        d_ptr->retBtn = new Button("", QIcon("/usr/local/nPM/icons/BackLeft.png"));
        d_ptr->retBtn->setButtonStyle(Button::ButtonIconOnly);
        d_ptr->retBtn->setIconSize(QSize(32, 32));
        d_ptr->retBtn->setMinimumWidth(100);
        bottomLayout->addWidget(d_ptr->retBtn);
        d_ptr->rightLayout->addLayout(bottomLayout);
        connect(d_ptr->retBtn, SIGNAL(clicked()), this, SLOT(onReturnBtnClick()));
    }
    Window::showEvent(ev);
}

void MenuWindow::onSelectItemChanged(int index)
{
    Q_ASSERT(index < d_ptr->stackWidget->count());

    d_ptr->stackWidget->setCurrentIndex(index);

    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        d_ptr->stackWidget->setFocusProxy(area);
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            if (d_ptr->menuPath.isEmpty())
            {
                setWindowTitle(content->description());
            }
            else
            {
                QString windowTitle = d_ptr->menuPath;
                windowTitle += "-";
                windowTitle += content->description();
                setWindowTitle(windowTitle);
            }
            content->setFocus();
            if (area->isAncestorOf(content->focusWidget()))
            {
                area->ensureWidgetVisible(content->focusWidget());
            }
        }
    }
}

void MenuWindow::onReturnBtnClick()
{
    // set focus back to the menubar
    QWidget *f = d_ptr->sidebar->getChecked();
    if (f)
    {
        f->setFocus();
    }
}
