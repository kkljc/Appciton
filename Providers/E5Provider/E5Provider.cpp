#include "E5Provider.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include "RTypeDefine.h"
#include "PatientDefine.h"
#include "ECGAlg2SoftInterface.h"
#include "Uart.h"
#include "ECGParam.h"
#include "ECGAlarm.h"
#include "RESPAlarm.h"
#include "SystemManager.h"
#include <Debug.h>
#include <QTextStream>
#include <QFile>

enum E5RecvPacketType
{
    E5_RSP_ACK                  = 0x01,         // ack for command

    E5_RSP_VERSION              = 0x11,         // version response
    E5_RSP_SELFTEST_RESULT      = 0x13,         // selftest result
    E5_RSP_LEAD_MODE            = 0x31,         // response of set mode
    E5_RSP_12LEAD_SUPPORT       = 0x33,         // response of get version
    E5_RSP_RESP_CALC_LEAD       = 0x51,         // response of set resp calculate lead
    E5_RSP_RESP_APNEA_TIME      = 0x53,         // response of set resp apnea time
    E5_RSP_ENABLE_RESP_CALC     = 0x55,         // response of enable resp calculate

    E5_NOTIFY_SYSTEM_START      = 0x80,         // module start notification
    E5_NOTIFY_RESP_ALARM        = 0x81,         // notify resp alarm status

    E5_PERIODIC_ALIVE           = 0xB0,         // module alive packet, receive frequency will be 1Hz
    E5_PERIODIC_ECG_DATA        = 0xBA,         // ecg data
    E5_PERIODIC_RESP_DATA       = 0xD0,         // resp data
    E5_PERIODIC_RR              = 0xD1,         // respiration rate

    E5_STATUS_ERROR_OR_WARN     = 0xF0,         // error or warning

};

enum E5SendPacketType
{
    E5_CMD_GET_VERSION          = 0x10,         // get version
    E5_CMD_GET_SELFTEST_RESULT  = 0x12,         // get system test result
    E5_CMD_SET_LEAD_MODE        = 0x30,         // set lead mode
    E5_CMD_CHECK_12LEAD_SUPPORT = 0x32,         // check 12 lead support
    E5_CMD_SET_RESP_CALC_LEAD   = 0x50,         // set resp calculate lead
    E5_CMD_SET_RESP_APNEA_TIME  = 0x52,         // set resp apnea time
    E5_CMD_ENABLE_RESP_CALC     = 0x54,         // enable resp calculation

    E5_CMD_UPGRADE              = 0xF6,         // enter upgrade mode
};

class E5ProviderPrivate
{
public:
    E5ProviderPrivate(E5Provider * const q_ptr)
        :q_ptr(q_ptr),
          ecgAlgInterface(getEcgInterface()),
          lastHR(0),
          support12Lead(false),
          ecgLeadMode(ECG_LEAD_MODE_5),
          respApneaTime(APNEA_ALARM_TIME_OFF),
          resplead(RESP_LEAD_II),
          enableRespCalc(false)
    {
        qMemSet(selftestResult, 0, sizeof(selftestResult));
    }

    void handleEcgRawData(unsigned char *data, int len);
    void handleRESPRawData(unsigned char *data, int len);

    E5Provider * const q_ptr;
    ECGInterfaceIFace *ecgAlgInterface;
    short lastHR;
    short lastLeadOff;
    bool support12Lead;
    char selftestResult[8];
    ECGLeadMode ecgLeadMode;
    ApneaAlarmTime respApneaTime;
    RESPLead resplead;
    bool enableRespCalc;
};

/* parse the data from the module and pass it to the algorithm interface */
void E5ProviderPrivate::handleEcgRawData(unsigned char *data, int len)
{
    Q_ASSERT(len >= 32);
    int ecgData[9];
    short ipace;
    short leadoff;
    int offset = 0;


    for(int i = 0; i < 9; i++)
    {
        ecgData[i] = data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16);
        //ecgData[i] = (data[offset] << 16) | (data[offset + 1] << 8) | data[offset + 2];
        offset += 3;
    }

    ipace = data[offset]  | (data[offset + 1] << 8) | (data[offset + 2] << 16);
    offset += 3;

    leadoff = data[offset] | (data[offset +1] << 8);


    // handle in algorithm interface
    ecgAlgInterface->setHandleData(ipace, leadoff, ecgData);

    short newHR = ecgAlgInterface->getHR();

    if(newHR != lastHR)
    {
        lastHR = newHR;
        ecgParam.updateHR(newHR);
    }

    int algleadData[ECG_LEAD_NR] = {0};
    bool qrsFlag;
    bool overload;
    bool leadOFFStatus[ECG_LEAD_NR];

    ecgAlgInterface->getWaveform(qrsFlag, overload, leadoff, ipace, algleadData);

    //need to swap some field
    int leadData[ECG_LEAD_NR];
    leadData[ECG_LEAD_I] = algleadData[E5ECGALG::I];
    leadData[ECG_LEAD_II] = algleadData[E5ECGALG::II];
    leadData[ECG_LEAD_III] = algleadData[E5ECGALG::III];
    leadData[ECG_LEAD_AVR] = algleadData[E5ECGALG::aVR];
    leadData[ECG_LEAD_AVL] = algleadData[E5ECGALG::aVL];
    leadData[ECG_LEAD_AVF] = algleadData[E5ECGALG::aVF];
    leadData[ECG_LEAD_V1] = algleadData[E5ECGALG::V1];
    leadData[ECG_LEAD_V2] = algleadData[E5ECGALG::V2];
    leadData[ECG_LEAD_V3] = algleadData[E5ECGALG::V3];
    leadData[ECG_LEAD_V4] = algleadData[E5ECGALG::V4];
    leadData[ECG_LEAD_V5] = algleadData[E5ECGALG::V5];
    leadData[ECG_LEAD_V6] = algleadData[E5ECGALG::V6];

    //static bool initFlag =false;
    //static QFile * f[ECG_LEAD_NR];
    //static QTextStream *stream[ECG_LEAD_NR];
    //if(!initFlag)
    //{
    //    initFlag = true;
    //    for(int i = 0; i < ECG_LEAD_NR; i++)
    //    {
    //        QString name = "/tmp/WAVE_";
    //        name += ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI);
    //        f[i] = new QFile(name);
    //        f[i]->open(QIODevice::WriteOnly);
    //        stream[i] = new QTextStream(f[i]);
    //    }
    //}

    //for(int i= 0 ; i< ECG_LEAD_NR; i++)
    //{
    //    *stream[i] << leadData[i]<<endl;
    //}


    leadOFFStatus[ECG_LEAD_I] = leadoff & (1 << E5ECGALG::I);
    leadOFFStatus[ECG_LEAD_II] = leadoff & (1 << E5ECGALG::II);
    leadOFFStatus[ECG_LEAD_III] = leadoff & (1 << E5ECGALG::III);
    leadOFFStatus[ECG_LEAD_V1] = leadoff & (1 << E5ECGALG::V1);
    leadOFFStatus[ECG_LEAD_V2] = leadoff & (1 << E5ECGALG::V2);
    leadOFFStatus[ECG_LEAD_V3] = leadoff & (1 << E5ECGALG::V3);
    leadOFFStatus[ECG_LEAD_V4] = leadoff & (1 << E5ECGALG::V4);
    leadOFFStatus[ECG_LEAD_V5] = leadoff & (1 << E5ECGALG::V5);
    leadOFFStatus[ECG_LEAD_V6] = leadoff & (1 << E5ECGALG::V6);
    leadOFFStatus[ECG_LEAD_AVR] = leadoff & (1 << E5ECGALG::aVR);
    leadOFFStatus[ECG_LEAD_AVL] = leadoff & (1 << E5ECGALG::aVL);
    leadOFFStatus[ECG_LEAD_AVF] = leadoff & (1 << E5ECGALG::aVF);

    if(lastLeadOff != leadoff )
    {
        lastLeadOff = leadoff;

        for(int i=0; i< ECG_LEAD_NR; i++)
        {
            ecgParam.setLeadOff((ECGLead)i, leadOFFStatus[i]);
        }
    }

    ecgParam.setOverLoad(overload);
    ecgParam.updateWaveform(leadData, leadOFFStatus, ipace, false, qrsFlag);

}

void E5ProviderPrivate::handleRESPRawData(unsigned char *data, int len)
{
    bool leadOff = data[0] & 0x80;
    int resp;
    int flag = 0;
    if(leadOff)
    {
        flag |= INVALID_WAVE_FALG_BIT;
        resp = q_ptr->getRESPBaseLine();
        respParam.setLeadoff(true);
    }
    else
    {
        short waveValue = ((data[0] & 0x7F) << 8) | data[1];

        if (waveValue & 0x4000)
        {
            //negative value
            waveValue |= 0x8000;
        }
        resp = waveValue;
        respParam.setLeadoff(false);
    }
    respParam.addWaveformData(resp, flag);
}

E5Provider::E5Provider()
    :BLMProvider("BLM_E5"),
      d_ptr(new E5ProviderPrivate(this))
{
    UartAttrDesc portAttr(460800, 8, 'N', 1, 0, FlOW_CTRL_HARD);
    initPort(portAttr);
    d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::II);
    d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_MONITOR);
    d_ptr->ecgAlgInterface->setLeadMode(E5ECGALG::ECG_MODE_5_LEAD);
    d_ptr->ecgAlgInterface->setNotchFilt(E5ECGALG::ECG_NOTCH_60_HZ);
    d_ptr->ecgAlgInterface->setPatientType(E5ECGALG::PATIENT_AUDLT);
    d_ptr->ecgAlgInterface->setPaceOnOff(true);

    //get selftest result
    sendCmd(E5_CMD_GET_SELFTEST_RESULT, NULL, 0);

    //get 12 Lead support status
    sendCmd(E5_CMD_CHECK_12LEAD_SUPPORT, NULL, 0);
}

E5Provider::~E5Provider()
{

}

bool E5Provider::attachParam(Param &param)
{
    bool ret = false;
    if(param.getParamID() == PARAM_ECG)
    {
        //TODO: fix the relation between param and provider, should use the abstrace class to
        //      set the provider, in other word, setprovider should be virtual.
        ecgParam.setProvider(this);
        ret = true;
    }
    else if(param.getParamID() == PARAM_RESP)
    {
        respParam.setProvider(this);
        ret = true;
    }

    return ret;

}

void E5Provider::detachParam(Param &param)
{
    //TODO
}

void E5Provider::handlePacket(unsigned char *data, int len)
{
    BLMProvider::handlePacket(data, len);

    switch (data[0])
    {
    case E5_RSP_ACK:
        break;
    case E5_RSP_VERSION:
        break;
    case E5_RSP_SELFTEST_RESULT:
        qMemCopy(d_ptr->selftestResult, data+1, sizeof(d_ptr->selftestResult));
        break;
    case E5_RSP_LEAD_MODE:
        d_ptr->ecgAlgInterface->setLeadMode((ECGMode) data[1]);
        break;
    case E5_RSP_12LEAD_SUPPORT:
        d_ptr->support12Lead = data[2];
        break;
    case E5_RSP_RESP_CALC_LEAD:
        break;
    case E5_RSP_RESP_APNEA_TIME:
        break;
    case E5_RSP_ENABLE_RESP_CALC:
        break;
    case E5_NOTIFY_SYSTEM_START:
    {
        //module restart, perfrom initialize

        //get selftest result
        sendCmd(E5_CMD_GET_SELFTEST_RESULT, NULL, 0);

        // get 12 Lead support status
        sendCmd(E5_CMD_CHECK_12LEAD_SUPPORT, NULL, 0);

        //set lead mode again
        setLeadSystem(d_ptr->ecgLeadMode);

        //resp setting
        setRESPCalcLead(d_ptr->resplead);
        //resp apnea time
        setApneaTime(d_ptr->respApneaTime);
        //resp calculation
        enableRESPCalc(d_ptr->enableRespCalc);
    }
        break;
    case E5_NOTIFY_RESP_ALARM:
        //apnea alarm status
        if(data[1])
        {
            respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_APNEA, true);
        }
        else
        {
            respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_APNEA, false);
        }
        break;
    case E5_PERIODIC_ALIVE:
        feed();
        break;
    case E5_PERIODIC_ECG_DATA:
        d_ptr->handleEcgRawData(data + 1, len - 1);
        break;
    case E5_PERIODIC_RESP_DATA:
        d_ptr->handleRESPRawData(data + 1, len - 1);
        break;
    case E5_PERIODIC_RR:
    {
        short rr = data[1] + (data[2] << 8);
        if(rr == -1)
        {
            rr = InvData();
        }
        respParam.setRR(rr);
    }
        break;
    case E5_STATUS_ERROR_OR_WARN:
        break;
    default:
        qdebug("unknown packet type 0x%02x", data[0]);
        break;
    }
}

void E5Provider::sendVersion()
{
    sendCmd(E5_CMD_GET_VERSION, NULL, 0);
}

/* module disconnect */
void E5Provider::disconnected()
{
    ecgOneShotAlarm.clear();
    respOneShotAlarm.clear();
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    for(int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        ecgParam.setLeadOff((ECGLead)i, true);
    }
    respParam.setLeadoff(true);

    systemManager.setPoweronTestResult(E5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
}


void E5Provider::reconnected()
{
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
}

int E5Provider::getWaveformSample()
{
    return 250;
}

void E5Provider::setWaveformSample(int rate)
{
    //TODO
}

void E5Provider::get05mV(int &p05mv, int &n05mv)
{
    //about 1.224 uV per unit
    p05mv = 408;
    n05mv = -408;
}

void E5Provider::getLeadCabelType()
{
    //TODO
}

void E5Provider::setLeadSystem(ECGLeadMode leadSystem)
{
    d_ptr->ecgLeadMode = leadSystem;

    switch(leadSystem)
    {
    case ECG_LEAD_MODE_3:
        d_ptr->ecgAlgInterface->setLeadMode(E5ECGALG::ECG_MODE_3_LEAD);
            break;
    case ECG_LEAD_MODE_12:
        d_ptr->ecgAlgInterface->setLeadMode(E5ECGALG::ECG_MODE_12_LEAD);
        break;
    case ECG_LEAD_MODE_5:
    default:
        d_ptr->ecgAlgInterface->setLeadMode(E5ECGALG::ECG_MODE_5_LEAD);
        break;
    }

    unsigned char dataPayload = (unsigned char) leadSystem;
    sendCmd(E5_CMD_SET_LEAD_MODE, &dataPayload, 1);
}

void E5Provider::setCalcLead(ECGLead lead)
{
    switch(lead)
    {
    case ECG_LEAD_I:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::I);
        break;
    case ECG_LEAD_II:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::II);
        break;
    case ECG_LEAD_III:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::III);
        break;
    case ECG_LEAD_AVR:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::aVR);
        break;
    case ECG_LEAD_AVL:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::aVL);
        break;
    case ECG_LEAD_AVF:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::aVF);
        break;
    case ECG_LEAD_V1:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V1);
        break;
    case ECG_LEAD_V2:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V2);
        break;
    case ECG_LEAD_V3:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V3);
        break;
    case ECG_LEAD_V4:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V4);
        break;
    case ECG_LEAD_V5:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V5);
        break;
    case ECG_LEAD_V6:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::V6);
        break;
    default:
        d_ptr->ecgAlgInterface->setCalLead(E5ECGALG::II);
        break;
    }
}

void E5Provider::setPatientType(unsigned char type)
{
    switch(type)
    {
    case PATIENT_TYPE_PED:
        d_ptr->ecgAlgInterface->setPatientType(E5ECGALG::PATIENT_PEDIATRIC);
        break;
    case PATIENT_TYPE_NEO:
        d_ptr->ecgAlgInterface->setPatientType(E5ECGALG::PATIENT_NEONATE);
        break;
    case PATIENT_TYPE_ADULT:
    default:
        d_ptr->ecgAlgInterface->setPatientType(E5ECGALG::PATIENT_AUDLT);
        break;
    }
}

void E5Provider::setBandwidth(ECGBandwidth bandwidth)
{
    switch(bandwidth)
    {
    case ECG_BANDWIDTH_067_20HZ:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_SURGERY);
        break;
    case ECG_BANDWIDTH_067_40HZ:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_MONITOR);
        break;
    case ECG_BANDWIDTH_0525_40HZ:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_ST);
        break;
    case ECG_BANDWIDTH_0525_150HZ:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_DIAGNOSTIC);
    default:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_MONITOR);
        break;
    }
}

void E5Provider::setFilterMode(ECGFilterMode mode)
{
    switch(mode)
    {
    case ECG_FILTERMODE_MONITOR:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_MONITOR);
        break;
    case ECG_FILTERMODE_DIAGNOSTIC:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_DIAGNOSTIC);
        break;
    case ECG_FILTERMODE_SURGERY:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_SURGERY);
        break;
    case ECG_FILTERMODE_ST:
        d_ptr->ecgAlgInterface->setEcgMode(E5ECGALG::ECG_FILTER_ST);
        break;
    default:
        qdebug("Unknown filter mode.");
        break;
    }
}

void E5Provider::enablePacermaker(ECGPaceMode onoff)
{
    d_ptr->ecgAlgInterface->setPaceOnOff(onoff == ECG_PACE_ON);
}

void E5Provider::setNotchFilter(ECGNotchFilter notch)
{
    switch (notch) {
    case ECG_NOTCH_50HZ:
        d_ptr->ecgAlgInterface->setNotchFilt(E5ECGALG::ECG_NOTCH_50_HZ);
        break;
    case ECG_NOTCH_60HZ:
        d_ptr->ecgAlgInterface->setNotchFilt(E5ECGALG::ECG_NOTCH_60_HZ);
        break;
    case ECG_NOTCH_50_AND_60HZ:
        d_ptr->ecgAlgInterface->setNotchFilt(E5ECGALG::ECG_NOTCH_5060_HZ);
        break;
    case ECG_NOTCH_OFF1:
    default:
        d_ptr->ecgAlgInterface->setNotchFilt(E5ECGALG::ECG_NOTCH_OFF);
        break;
    }
}

void E5Provider::enableSTAnalysis(bool onoff)
{

    //TODO
}

void E5Provider::setSTPoints(int iso, int st)
{

    //TODO
}

int E5Provider::getRESPWaveformSample()
{
    return 125;
}

void E5Provider::disableApnea()
{
    d_ptr->respApneaTime = APNEA_ALARM_TIME_OFF;
    unsigned char apneaTime = d_ptr->respApneaTime;
    sendCmd(E5_CMD_SET_RESP_APNEA_TIME, &apneaTime, 1);
}

void E5Provider::setApneaTime(ApneaAlarmTime t)
{
    d_ptr->respApneaTime = t;
    unsigned char apneaTime = t;
    sendCmd(E5_CMD_SET_RESP_APNEA_TIME, &apneaTime, 1);
}

void E5Provider::setWaveformZoom(RESPZoom zoom)
{
    //TODO
}

void E5Provider::setRESPCalcLead(RESPLead lead)
{
    d_ptr->resplead == lead;
    unsigned char l = lead;
    sendCmd(E5_CMD_SET_RESP_CALC_LEAD, &l, 1);
}

void E5Provider::enableRESPCalc(bool enable)
{
    d_ptr->enableRespCalc = enable;
    unsigned char onOff = enable ? 1 : 0;
    sendCmd(E5_CMD_ENABLE_RESP_CALC, &onOff, 1);
}
