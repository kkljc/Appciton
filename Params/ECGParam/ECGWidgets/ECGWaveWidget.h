#pragma once
#include "WaveWidget.h"
#include "ParamInfo.h"
#include "ECGDefine.h"

//记录R波标记,用于重画，防止R波标记被刷新后只显示一半
struct rMark_record{
    rMark_record()
    {
        ellipse_x = 0;
        ellipse_y = 0;
        s_x = 0;
        s_y = 0;
    }

    int ellipse_x;
    int ellipse_y;
    int s_x;
    int s_y;
};

class WaveWidgetLabel;
class ECGWaveRuler;
class ComboListPopup;
/**************************************************************************************************
 * ECG 波形显示控件
 *************************************************************************************************/
class ECGWaveWidget : public WaveWidget
{
    Q_OBJECT
public:
    ECGWaveWidget(WaveformID id, const QString &widgetName, const QString &leadName);
    ~ECGWaveWidget();

    // 获取当前控件的高度，毫秒为单位。
    int getHeightMM(void);

    // 设置+/-0.5mv对应的数值。
    void set05MV(int p05mv, int n05mv);

    // 添加波形数据。
    void addWaveformData(int data, int pace = 0);

    // 设置波形增益。
    void setGain(ECGGain gain);

    //设置12导界面下波形增益
    void set12LGain(ECGGain gain);

    //获取12导界面下波形增益
    ECGGain get12LGain();

    // 设置波形速度。
    void setSpeed(ECGSweepSpeed speed);

    // 设置带宽
    void setBandWidth(ECGBandwidth bandWidth);

    // 获取导联带宽字符串
    QString getBandWidthStr();

    // 设置自动增益标志
    void setAutoGain(bool flag);

    // set wave notify message
    void setNotifyMesg(ECGWaveNotify mesg);

    //update the lead display name
    void updateLeadDisplayName(const QString &name);

    // 画ECG波形标记
    static void drawIPaceMark(QPainter &painter, QPoint &start, QPoint &end);

    static void drawEPaceMark(QPainter &painter, QPoint &start, QPoint &end, QRect &r,
                              unsigned flag, ECGWaveWidget *pObj = NULL);
    static void drawRMark(QPainter &painter, QPoint &p, QRect &r, ECGWaveWidget *pObj = NULL);

    //记录R波标记,用于重画，防止R波标记被刷新后只显示一半
    QList<rMark_record> rMarkList;

protected:
    // 重绘事件。
    virtual void paintEvent(QPaintEvent *e);

    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 显示
    virtual void showEvent(QShowEvent *e);

    // hide
    virtual void hideEvent(QHideEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);

private slots:
    // 鼠标释放事件
    void _releaseHandle(IWidget *);
    void _ecgGain(IWidget *);
    void _popupDestroyed();
    void _onCalcLeadChanged();

private:
    double _calcRulerHeight(ECGGain gain);
    void _initValueRange(ECGGain gain);

    // 载入配置。
    void _loadConfig(void);

private:
//    WaveWidgetLabel *_name;         // 名称
    WaveWidgetLabel *_gain;         // 增益标签
    WaveWidgetLabel *_filter;       // 滤波标签
    WaveWidgetLabel *_notify;       // 提示标签
    ComboListPopup *_gainList;      // 增益列表
    static int _paceHeight;         // 起搏标记高度, 单位像素
    int _p05mV;                     // +0.5mV对应的波形数值
    int _n05mV;                     // -0.5mV对应的波形数值
    ECGGain _12LGain;               //12L界面下的显示带宽

private:
    void _autoGainHandle(int data);
    void _calcGainRange(void);

    bool _isAutoGain;
    unsigned _autoGainTime;
    int _autoGainTracePeek;
    int _autoGainTraveVally;
    ECGWaveNotify _mesg;
    struct autoGainLogicalRange
    {
        autoGainLogicalRange()
        {
            minRange = 0;
            maxRange = 0;
        }
        int minRange;
        int maxRange;
    };
    autoGainLogicalRange _autoGainLogicalRange[ECG_GAIN_NR - 1];
};