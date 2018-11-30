/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#include "HistoryDataSelModel.h"
#include "DataStorageDirManager.h"
#include "ThemeManager.h"
#include <QDateTime>
#include "TimeDate.h"
#include "WindowManager.h"

#define COLUMN_COUNT        2
#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class HistoryDataSelModelPrivate
{
public:
    HistoryDataSelModelPrivate()
        : row(0)
    {}

    /**
     * @brief _convertTimeStr 将时间字符串转换成显示可以的字符串
     * @param str 需要转换的字符串
     * @return
     */
    QString convertTimeStr(const QString str);
public:
    QList<QString> strList;
    int row;
    QList<QString> firstDataList;
    QList<QString> secondDataList;
};

HistoryDataSelModel::HistoryDataSelModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new HistoryDataSelModelPrivate())
{
}

HistoryDataSelModel::~HistoryDataSelModel()
{
}

int HistoryDataSelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int HistoryDataSelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->firstDataList.count();
}

QVariant HistoryDataSelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || d_ptr->strList.count() == 0)
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (column == 0)
        {
            return d_ptr->firstDataList.value(row, "");
        }
        else if (column == 1)
        {
            return d_ptr->secondDataList.value(row, "");
        }
        break;
    }
    case Qt::SizeHintRole:
    {
        int w = windowManager.getPopWindowWidth() / COLUMN_COUNT;
        return QSize(w, ROW_HEIGHT_HINT);
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags HistoryDataSelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    int row = index.row();
    int column = index.column();


    Qt::ItemFlags flags;
    int num = row * 2;
    num += (column == 0 ? 1 : 2);
    if (num > d_ptr->strList.count())
    {
        flags = Qt::NoItemFlags;
        return flags;
    }

    flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

int HistoryDataSelModel::getRowHeightHint()
{
    return ROW_HEIGHT_HINT;
}

void HistoryDataSelModel::updateData()
{
    beginResetModel();
    d_ptr->strList.clear();
    d_ptr->firstDataList.clear();
    d_ptr->secondDataList.clear();
    QStringList list;
    dataStorageDirManager.getRescueEvent(list);
    d_ptr->strList.append(list);

    int count = d_ptr->strList.count();
    d_ptr->row = count / 2;
    d_ptr->row += (count % 2) ? 1 : 0;
    for (int i = 0; i < d_ptr->row; i ++)
    {
        int index = i * 2;
        QString timeStr = d_ptr->convertTimeStr(d_ptr->strList.at(index));
        d_ptr->firstDataList.append(timeStr);

        index = i * 2 + 1;
        if (index > count - 1)
        {
            break;
        }
        timeStr = d_ptr->convertTimeStr(d_ptr->strList.at(index));
        d_ptr->secondDataList.append(timeStr);
    }
    endResetModel();
}

QString HistoryDataSelModel::getDateTimeStr(int index)
{
    QString str = d_ptr->strList.at(index);
    str = d_ptr->convertTimeStr(str);
    return str;
}

QString HistoryDataSelModelPrivate::convertTimeStr(const QString str)
{
    QString timeStr;
    QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
    timeDate.getDateTime(dt.toTime_t(), timeStr, true, true);
    return timeStr;
}
