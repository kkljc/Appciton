/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#pragma once
#include <QWidget>

class PopupListPrivate;
class PopupList : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief PopupList
     * @param concatToParent
     * @param parent
     */
    explicit PopupList(QWidget *parent = NULL, bool concatToParent = true);

    ~PopupList();

    /**
     * @brief addItemText add a popup list item with the @text
     * @param text item text
     */
    void addItemText(const QString &text);

    /**
     * @brief setCurrentIndex set the selected item, negative value meams uncheck
     * @param index
     */
    void setCurrentIndex(int index);

    /**
     * @brief getCurrentIndex get he current selected item
     * @return get the selected items, negative value means no selected item
     */
    int getCurrentIndex() const;

    /* reimplement */
    QSize sizeHint() const;

protected:
    /* reimplement */
    void showEvent(QShowEvent *e);
    /* reimplement */
    void paintEvent(QPaintEvent *e);
    /* reimplement */
    void resizeEvent(QResizeEvent *e);

    /* reimplement */
    void keyPressEvent(QKeyEvent *e);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *e);

    /* reimplement */
    bool focusNextPrevChild(bool next);

signals:
    void selectItemChanged(int index);

private slots:
    void onItemSelected();

private:
    QScopedPointer<PopupListPrivate> d_ptr;
};