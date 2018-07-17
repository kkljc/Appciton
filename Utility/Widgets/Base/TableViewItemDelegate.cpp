/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/11
 **/

#include "TableViewItemDelegate.h"
#include "PopupList.h"
#include <QPainter>
#include "ThemeManager.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTableView>
#include "ItemEditInfo.h"
#include "PopupNumEditor.h"
#include "FontManager.h"
#include <QDebug>

#define MARGIN 2

class TableViewItemDelegatePrivate
{
public:
    TableViewItemDelegatePrivate()
        : curEditingModel(NULL)
    {
    }

    const QWidget *widget(const QStyleOptionViewItem &option) const
    {
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
        {
            return v3->widget;
        }

        return 0;
    }

    QModelIndex curPaintingIndex;   // record current painting item's index
    QModelIndex curEditingIndex;    // record current painting item's index
    QAbstractItemModel *curEditingModel; // current editing model
    Qt::CheckState checkState;  // record current item's check state
    QPalette pal;   // palette to used when draw check state
};

TableViewItemDelegate::TableViewItemDelegate(QObject *parent)
    : QItemDelegate(parent), d_ptr(new TableViewItemDelegatePrivate())
{
    themeManger.setupPalette(ThemeManager::ControlComboBox, d_ptr->pal);
}

TableViewItemDelegate::~TableViewItemDelegate()
{
    delete d_ptr;
}

QWidget *TableViewItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    qDebug() << Q_FUNC_INFO << index.row();
    return QItemDelegate::createEditor(parent, option, index);
}

void TableViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // get the model index
    d_ptr->curPaintingIndex = index;
    QItemDelegate::paint(painter, option, index);
}

/**
 * In the QItemDelegate::paint, the api is call int the folllowing order:
 *   drawBackground (non virtual)
 *   drawCheck (virtual)
 *   drawDecoraion (virtual)
 *   drawFocus (virtual)
 *
 * when the item is editable, we use the drawcheck to draw the background and border. when the item is editable,
 * the model should also provider the Qt::Checkablerole
 */
void TableViewItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
                                      Qt::CheckState state) const
{
    d_ptr->checkState = state;
    if (!rect.isValid() || !(option.state & QStyle::State_Enabled))
    {
        // when uncheckable, the rect will be zero
        return;
    }

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    // draw the item background color
    QVariant value = d_ptr->curPaintingIndex.data(Qt::BackgroundRole);
    if (value.canConvert<QBrush>())
    {
        QPointF oldBO = painter->brushOrigin();
        painter->setBrushOrigin(option.rect.topLeft());
        painter->fillRect(option.rect, qvariant_cast<QBrush>(value));
        painter->setBrushOrigin(oldBO);
    }

    // draw the background and border
    QColor borderColor;
    QColor bgColor;
    if (option.state & QStyle::State_HasFocus)
    {
        borderColor = d_ptr->pal.color(QPalette::Active, QPalette::Shadow);
        bgColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Window);
    }
    else
    {
        borderColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Shadow);
        bgColor = d_ptr->pal.color(QPalette::Inactive, QPalette::Window);
    }

    int borderWidth = themeManger.getBorderWidth();
    int radius = themeManger.getBorderRadius();
    QRect r = option.rect.adjusted(MARGIN, 0, -MARGIN, 0);
    r.adjust(borderWidth / 2, borderWidth / 2, -borderWidth / 2, -borderWidth / 2);
    QPen pen(borderColor, borderWidth);
    painter->setPen(pen);
    painter->setBrush(bgColor);
    if (state == Qt::Checked)
    {
        // NOTE: we only draw the upper border radius here. This is fine for the popuplist widget, because the
        // popuplist will concat to the item border; It's not so correct for the popupnumeditor, becacuse the
        // popupnumeditor will not concat to the item border but overlay the item. So here is fine, we can't see
        // the item when the popupnumeditor is shown. We can do some optimization here, fix it later.
        painter->setClipRect(option.rect);
        r.adjust(0, 0, 0, radius + borderWidth);
    }
    painter->drawRoundedRect(r, radius, radius);
    painter->restore();
}

void TableViewItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(rect)
}

void TableViewItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
                                        const QString &text) const
{
    if (d_ptr->checkState == Qt::Unchecked)
    {
        QItemDelegate::drawDisplay(painter, option, rect, text);
    }
    else
    {
        QStyleOptionViewItem opt(option);
        opt.state &= (~QStyle::State_Selected);
        QRect r = QItemDelegate::rect(opt, d_ptr->curPaintingIndex, Qt::DisplayRole);
        r = QStyle::alignedRect(option.direction, option.displayAlignment, r.size().boundedTo(option.rect.size()), option.rect);
        QItemDelegate::drawDisplay(painter, opt, r, text);
    }
}

bool TableViewItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                        const QModelIndex &index)
{

    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is editable
    Qt::ItemFlags flags = model->flags(index);
    if (!(option.state & QStyle::State_Enabled)
            || !(flags & Qt::ItemIsEditable)
            || !(flags & Qt::ItemIsEnabled))
    {
        return false;
    }

    // make sure we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
    {
        return false;
    }

    const QTableView *view = qobject_cast<const QTableView *>(d_ptr->widget(option));
    if (!view)
    {
        // can't access the view
        return false;
    }

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
            || (event->type() == QEvent::MouseButtonDblClick)
            || (event->type() == QEvent::MouseButtonPress))
    {

        d_ptr->curEditingModel = model;
        d_ptr->curEditingIndex = index;

        model->setData(index, QVariant(Qt::Checked), Qt::CheckStateRole);
        QVariant value = model->data(index, Qt::EditRole);
        if (value.canConvert<ItemEditInfo>())
        {

            ItemEditInfo info = qvariant_cast<ItemEditInfo>(value);

            QRect vrect = view->visualRect(index);
            QRect rect(view->viewport()->mapToGlobal(vrect.topLeft()),
                       view->viewport()->mapToGlobal(vrect.bottomRight()));

            rect.adjust(MARGIN, 0, -MARGIN, 0);
            if (info.type == ItemEditInfo::LIST)
            {
                PopupList *popup = new PopupList();
                popup->setFixedWidth(rect.width());
                popup->additemList(info.list);
                popup->move(rect.bottomLeft());
                connect(popup, SIGNAL(destroyed(QObject *)), this, SLOT(onPopupDestroy()));
                popup->show();
                return true;
            }
            else if (info.type == ItemEditInfo::VALUE)
            {
                PopupNumEditor *editor = new PopupNumEditor();
                editor->setEditInfo(info);
                editor->setFont(fontManager.textFont(view->font().pixelSize()));
                editor->setPalette(d_ptr->pal);
                editor->setEditValueGeometry(rect);
                connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(onPopupDestroy()));
                editor->show();
                return true;
            }
        }
    }
    else if (event->type() == QEvent::KeyPress)
    {
    }
    else
    {
        return false;
    }

    return false;
}

void TableViewItemDelegate::onPopupDestroy()
{
    if (d_ptr->curEditingIndex.isValid())
    {
        d_ptr->curEditingModel->setData(d_ptr->curEditingIndex, QVariant(Qt::PartiallyChecked), Qt::CheckStateRole);
        d_ptr->curEditingIndex = QModelIndex();
        d_ptr->curEditingModel = NULL;
    }
}
