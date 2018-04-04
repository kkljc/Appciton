#pragma once
#include "IWidget.h"
#include "AlarmDefine.h"

class AlarmInfoPopListVIew;
class AlarmPhyInfoBarWidget : public IWidget
{
    Q_OBJECT

public:
    // 清除界面。
    void clear(void);

    // 设置报警提示信息。
    void display(AlarmInfoNode &node);

    //返回对象本身
    static AlarmPhyInfoBarWidget &getSelf();

    // 构造与析构。
    AlarmPhyInfoBarWidget(const QString &name);
    ~AlarmPhyInfoBarWidget();

    void updateList();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _releaseHandle(IWidget *);
    void _alarmListHide();

private:
    void _drawBackground(void);
    void _drawText(void);

    AlarmType _type;
    AlarmPriority _alarmPriority;
    unsigned _pauseTime;
    QString _text;
    bool _latch;
    bool _acknowledge;

    AlarmInfoPopListVIew *_alarmList;
    AlarmType _alarmType;
};
#define alarmPhyInfoBarWin (AlarmPhyInfoBarWidget::getSelf())