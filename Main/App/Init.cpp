#include "Init.h"
#include "ErrorLogItem.h"

/**************************************************************************************************
 * 功能： 初始化系统。
 *************************************************************************************************/
static void _initSystem(void)
{
    // 配置初始化。
    machineConfig.construction();

    systemConfig.construction();

    superConfig.construction();

    superRunConfig.construction();

    // 新会话，需要恢复主配置文件
    if (timeManager.getPowerOnSession() == POWER_ON_SESSION_NEW)
    {
        superRunConfig.load(SUPERVISOR_CFG_FILE);

        Config systemDefCfg(ORGINAL_SYSTEM_CFG_FILE);
        systemConfig.setNodeValue("PrimaryCfg", systemDefCfg);

        ErrorLogItem *item = new ErrorLogItem();
        item->setName("Load default config");
        QString str = SUPERVISOR_CFG_FILE;
        str += "\r\n";
        str += ORGINAL_SYSTEM_CFG_FILE;
        str += "\r\n";
        item->setLog(str);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
    }

    // 波形缓存。
    waveformCache.construction();

    // tick初始化。
    systemTick.construction();

    // 初始化声音。
    soundManager.construction();
    soundManager.selfTest();

    // 网络初始化。
//    networkManager.construction();
    mailManager.construction();

    // 按键板初始化。
//    systemBoardProvider.construction();

    // 报警灯。
    lightManager.construction();
    lightManager.setProvider(&systemBoardProvider);
    systemManager.enableBrightness(systemManager.getBrightness());

    // 自检
    systemBoardProvider.selfTest();

    // 存储目录管理。
    dataStorageDirManager.construction();

    //消息提示框
    pMessageBox.construction();
}

/**************************************************************************************************
 * 功能： 初始化界面相关。
 *************************************************************************************************/
static void _initWindowManager(void)
{
    windowManager.construction();
    waveWidgetSelectMenu.construction();

    // Set window size
    int screenWidth = 0;
    int screenHeight = 0;
    machineConfig.getNumValue("ScreenWidth", screenWidth);
    machineConfig.getNumValue("ScreenHeight", screenHeight);
    windowManager.setFixedSize(screenWidth, screenHeight);
    windowManager.setPopMenuSize(screenWidth, screenHeight);
}

/**************************************************************************************************
 * 功能： 初始化报警。
 *************************************************************************************************/
static void _initComponents(void)
{
    keyActionManager.Construction();
    keyActionManager.registerKetAction(new NormalModeKeyAction());

    // Softkeys
    softkeyManager.construction();
    windowManager.addWidget(&softkeyManager);

    // 时间管理功能初始化。
    DateTimeWidget *widget0 = new DateTimeWidget();
    ElapseTimeWidget *widget1 = new ElapseTimeWidget();
    windowManager.addWidget(widget0);
    windowManager.addWidget(widget1);
    timeManager.registerWidgets(*widget0, *widget1);

    // 基础时间日期管理。
    timeDate.construction();

    // 提示信息烂
    PromptInfoBarWidget *promptInfo = new PromptInfoBarWidget("PromptInfoBarWidget");
    windowManager.addWidget(promptInfo);
    ibpParam.setInfobarWidget(promptInfo);

    // 系统状态栏
    windowManager.addWidget(&sysStatusBar.construction());
    windowManager.addWidget(new SystemModeBarWidget());

    // 电源
    BatteryBarWidget *bar = &batteryBarWidget.construction();
    powerManager.construction();
    alertor.addOneShotSource(batteryOneShotAlarm.construction());
    windowManager.addWidget(bar);

    // 病人管理初始化。
    PatientInfoWidget *patientInfoWidget = new PatientInfoWidget();
    windowManager.addWidget(patientInfoWidget);
    patientManager.setPatientInfoWidget(*patientInfoWidget);
    PatientBarWidget *patientTypeWidget = new PatientBarWidget();
    windowManager.addWidget(patientTypeWidget);
    patientManager.setPatientTypeWidget(*patientTypeWidget);
    windowManager.addWidget(&patientStatusBar.construction());

    // 初始化报警。
    alertor.construction();
    alarmIndicator.construction();

    AlarmPhyInfoBarWidget *alarmPhyInfo = new AlarmPhyInfoBarWidget("AlarmPhyInfoBarWidget");
    AlarmTechInfoBarWidget *alarmTechInfo = new AlarmTechInfoBarWidget("AlarmTechInfoBarWidget");
    AlarmMuteBarWidget *alarmPhyMuteBar = new AlarmMuteBarWidget();
    alarmIndicator.setAlarmPhyWidgets(alarmPhyInfo, alarmPhyMuteBar);
    alarmIndicator.setAlarmTechWidgets(alarmTechInfo);
//    windowManager.addWidget(new AlarmInfoBarIndicator());
    windowManager.addWidget(alarmPhyInfo);
    windowManager.addWidget(alarmTechInfo);
    windowManager.addWidget(alarmPhyMuteBar);

    //报警状态
    alarmStateMachine.Construction();
    alarmStateMachine.start();

    // 数据管理。
    paramDataStorageManager.construction();
    summaryStorageManager.construction();
    ecg12LDataStorage.construction();
    summaryRescueDataWidget.construction();
    summaryRescueRangePrint.construction();
    rescueDataExportWidget.construction();
    rescueDataDeleteWidget.construction();

    //U盘管理
    usbManager.getInstance();
    //U盘数据储存
    rawDataCollection.construction();
    rawDataCollectionTxt.construction();

    //12导
    ecg12LDataPrint.construction();
    ecg12LeadManager.construction();
}

/**************************************************************************************************
 * 功能： 初始化参数和提供者对象。
 *************************************************************************************************/
static void _initProviderParam(void)
{
    paramInfo.construction();

    // 创建Provider.
    paramManager.addProvider(*new DemoProvider());
    //TE3Provider *te3 = new TE3Provider();
    //paramManager.addProvider(*te3);

    E5Provider *e5 = new E5Provider();
    paramManager.addProvider(*e5);


    // ECG部分。
    paramManager.addParam(ecgDupParam.construction());
    alertor.addLimtSource(ecgDupLimitAlarm.construction());
    paramManager.addParam(ecgParam.construction());
    alertor.addLimtSource(ecgLimitAlarm.construction());
    alertor.addOneShotSource(ecgOneShotAlarm.construction());
    ECGTrendWidget *ecgTrendWidget = new ECGTrendWidget();
    ecgDupParam.setTrendWidget(ecgTrendWidget);
    windowManager.addWidget(ecgTrendWidget);
    ECGPVCSTrendWidget *ecgPVCSTrendWidget = new ECGPVCSTrendWidget();
    ecgParam.setECGPVCSTrendWidget(ecgPVCSTrendWidget);
    windowManager.addWidget(ecgPVCSTrendWidget);
    ECGSTTrendWidget *ecgSTTrendWidget = new ECGSTTrendWidget();
    ecgParam.setECGSTTrendWidget(ecgSTTrendWidget);
    windowManager.addWidget(ecgSTTrendWidget);

    ECGWaveWidget *ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_I, "ECGIWaveWidget",
        ECGSymbol::convert(ECG_LEAD_I, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_I);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_II, "ECGIIWaveWidget",
        ECGSymbol::convert(ECG_LEAD_II, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_II);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_III, "ECGIIIWaveWidget",
        ECGSymbol::convert(ECG_LEAD_III, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_III);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVR, "ECGaVRWaveWidget",
        ECGSymbol::convert(ECG_LEAD_AVR, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVR);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVL, "ECGaVLWaveWidget",
        ECGSymbol::convert(ECG_LEAD_AVL, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVL);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVF, "ECGaVFWaveWidget",
        ECGSymbol::convert(ECG_LEAD_AVF, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVF);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V1, "ECGV1WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V1, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V1);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V2, "ECGV2WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V2, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V2);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V3, "ECGV3WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V3, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V3);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V4, "ECGV4WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V4, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V4);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V5, "ECGV5WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V5, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V5);
    windowManager.addWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V6, "ECGV6WaveWidget",
        ECGSymbol::convert(ECG_LEAD_V6, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V6);
    windowManager.addWidget(ecgWaveWidget);

    OxyCRGWidget *oxyCRGWidget = new OxyCRGWidget();
    windowManager.addWidget(oxyCRGWidget);

    OxyCRGHRWidget *hrOxyCRGWidget = new OxyCRGHRWidget("OxyCRGHRWidget",
        paramInfo.getSubParamName(SUB_PARAM_HR_PR));
    ecgParam.setOxyCRGWaveWidget(hrOxyCRGWidget);
    windowManager.addWidget(hrOxyCRGWidget);

    // RESP部分。
    if (systemManager.isSupport(CONFIG_RESP) || systemManager.isSupport(CONFIG_CO2))
    {
        paramManager.addParam(respDupParam.construction());
        alertor.addLimtSource(respDupLimitAlarm.construction());
        if (systemManager.isSupport(CONFIG_RESP))
        {
            paramManager.addParam(respParam.construction());
            alertor.addLimtSource(respLimitAlarm.construction());
            alertor.addOneShotSource(respOneShotAlarm.construction());
            RESPWaveWidget *respWaveWidget = new RESPWaveWidget("RESPWaveWidget",
                paramInfo.getParamName(PARAM_RESP));
            RESPTrendWidget *respTrendWidget = new RESPTrendWidget();
            respDupParam.setTrendWidget(respTrendWidget);
            respParam.setWaveWidget(respWaveWidget);
            windowManager.addWidget(respWaveWidget);
            windowManager.addWidget(respTrendWidget);
        }
    }

    // SPO2部分。
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        QString str;
        machineConfig.getStrValue("SPO2", str);
        if (str == "BLM_SPO2")
        {
            paramManager.addProvider(*new BLMSPO2Provider());
        }
        else if (str == "MASIMO_SPO2")
        {
            paramManager.addProvider(*new MasimoSetProvider());
        }
        else
        {
            paramManager.addProvider(*new TS3Provider());
        }
        paramManager.addParam(spo2Param.construction());
        alertor.addLimtSource(spo2LimitAlarm.construction());
        alertor.addOneShotSource(spo2OneShotAlarm.construction());
        SPO2WaveWidget *spo2WaveWidget = new SPO2WaveWidget("SPO2WaveWidget", trs("PLETH"));
        SPO2TrendWidget *spo2TrendWidget = new SPO2TrendWidget();
        spo2Param.setTrendWidget(spo2TrendWidget);
        spo2Param.setWaveWidget(spo2WaveWidget);
        windowManager.addWidget(spo2WaveWidget);
        windowManager.addWidget(spo2TrendWidget);

        OxyCRGSPO2Widget *spo2OxyCRGWidget = new OxyCRGSPO2Widget("OxyCRGSPO2Widget",
            paramInfo.getSubParamName(SUB_PARAM_SPO2));
        spo2Param.setOxyCRGWaveWidget(spo2OxyCRGWidget);
        windowManager.addWidget(spo2OxyCRGWidget);
    }

    // NIBP部分。
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        QString str;
        machineConfig.getStrValue("NIBP", str);
        if (str == "SUNTECH_NIBP")
        {
            paramManager.addProvider(*new SuntechProvider());
        }
        else
        {
            paramManager.addProvider(*new TN3Provider());
        }

        paramManager.addParam(nibpParam.construction());
        alertor.addLimtSource(nibpLimitAlarm.construction());
        alertor.addOneShotSource(nibpOneShotAlarm.construction());
        NIBPTrendWidget *nibpTrenWidget = new NIBPTrendWidget();
        nibpParam.setNIBPTrendWidget(nibpTrenWidget);
        windowManager.addWidget(nibpTrenWidget);
        NIBPDataTrendWidget *nibpDataTrenWidget = new NIBPDataTrendWidget();
        nibpParam.setNIBPDataTrendWidget(nibpDataTrenWidget);
        windowManager.addWidget(nibpDataTrenWidget);
        WaveTrendWidget *waveTrendWidget = new WaveTrendWidget();
        windowManager.addWidget(waveTrendWidget);
    }

    // CO2部分。
    if (systemManager.isSupport(CONFIG_CO2))
    {
        paramManager.addProvider(*new BLMCO2Provider());

        paramManager.addParam(co2Param.construction());
        alertor.addLimtSource(co2LimitAlarm.construction());
        alertor.addOneShotSource(co2OneShotAlarm.construction());
        CO2WaveWidget *co2WaveWidget = new CO2WaveWidget("CO2WaveWidget",
                paramInfo.getParamName(PARAM_CO2));
        CO2TrendWidget *co2TrendWidget = new CO2TrendWidget();
        co2Param.setTrendWidget(co2TrendWidget);
        co2Param.setWaveWidget(co2WaveWidget);
        windowManager.addWidget(co2WaveWidget);
        windowManager.addWidget(co2TrendWidget);
    }

    //IBP test
    if (systemManager.isSupport(CONFIG_IBP))
    {
        paramManager.addProvider(*new WitleafProvider());
        paramManager.addParam(ibpParam.construction());
        alertor.addLimtSource(ibpLimitAlarm.construction());
        alertor.addOneShotSource(ibpOneShotAlarm.construction());

        IBPTrendWidget *ibp1TrendWidget = new IBPTrendWidget("IBP1TrendWidget",
                                                             IBP_PRESSURE_ART);
        ibpParam.setIBPTrendWidget(ibp1TrendWidget,IBP_INPUT_1);
        windowManager.addWidget(ibp1TrendWidget);

        IBPTrendWidget *ibp2TrendWidget = new IBPTrendWidget("IBP2TrendWidget",
                                                             IBP_PRESSURE_PA);
        ibpParam.setIBPTrendWidget(ibp2TrendWidget,IBP_INPUT_2);
        windowManager.addWidget(ibp2TrendWidget);

        IBPWaveWidget *ibp1WaveWidget = new IBPWaveWidget(WAVE_IBP1, "IBP1WaveWidget",
                                                          IBP_PRESSURE_ART);
        ibpParam.setWaveWidget(ibp1WaveWidget,IBP_INPUT_1);
        windowManager.addWidget(ibp1WaveWidget, ibp1TrendWidget);

        IBPWaveWidget *ibp2WaveWidget = new IBPWaveWidget(WAVE_IBP2, "IBP2WaveWidget",
                                                          IBP_PRESSURE_PA);
        ibpParam.setWaveWidget(ibp2WaveWidget,IBP_INPUT_2);
        windowManager.addWidget(ibp2WaveWidget, ibp2TrendWidget);
    }

    //CO
    if (systemManager.isSupport(CONFIG_CO))
    {
        paramManager.addParam(coParam.construction());
        alertor.addLimtSource(coLimitAlarm.construction());
        alertor.addOneShotSource(coOneShotAlarm.construction());
        COTrendWidget *coTrendWidget = new COTrendWidget("COTrendWidget");
        coParam.setCOTrendWidget(coTrendWidget);
        windowManager.addWidget(coTrendWidget);
    }

    //AG
    if (systemManager.isSupport(CONFIG_AG))
    {
        paramManager.addProvider(*new PhaseinProvider());
        paramManager.addParam(agParam.construction());
        alertor.addLimtSource(agLimitAlarm.construction());
        alertor.addOneShotSource(agOneShotAlarm.construction());
    }

    AGTrendWidget *trendWidgetN2O = new AGTrendWidget("N2OTrendWidget", AG_TYPE_N2O);
    agParam.setTrendWidget(trendWidgetN2O, AG_TYPE_N2O);
    windowManager.addWidget(trendWidgetN2O);

    AGTrendWidget *trendWidgetAA1 = new AGTrendWidget("AA1TrendWidget", AG_TYPE_AA1);
    agParam.setTrendWidget(trendWidgetAA1, AG_TYPE_AA1);
    windowManager.addWidget(trendWidgetAA1);

    AGTrendWidget *trendWidgetAA2 = new AGTrendWidget("AA2TrendWidget", AG_TYPE_AA2);
    agParam.setTrendWidget(trendWidgetAA2, AG_TYPE_AA2);
    windowManager.addWidget(trendWidgetAA2);

    AGTrendWidget *trendWidgetO2 = new AGTrendWidget("O2TrendWidget", AG_TYPE_O2);
    agParam.setTrendWidget(trendWidgetO2, AG_TYPE_O2);
    windowManager.addWidget(trendWidgetO2);

    AGWaveWidget *waveWidgetN2O = new AGWaveWidget(WAVE_N2O, "N2OWaveWidget", AG_TYPE_N2O);
    agParam.setWaveWidget(waveWidgetN2O, AG_TYPE_N2O);
    windowManager.addWidget(waveWidgetN2O, trendWidgetN2O);

    AGWaveWidget *waveWidgetAA1 = new AGWaveWidget(WAVE_AA1, "AA1WaveWidget", AG_TYPE_AA1);
    agParam.setWaveWidget(waveWidgetAA1, AG_TYPE_AA1);
    windowManager.addWidget(waveWidgetAA1, trendWidgetAA1);

    AGWaveWidget *waveWidgetAA2 = new AGWaveWidget(WAVE_AA2, "AA2WaveWidget", AG_TYPE_AA2);
    agParam.setWaveWidget(waveWidgetAA2, AG_TYPE_AA2);
    windowManager.addWidget(waveWidgetAA2, trendWidgetAA2);

    AGWaveWidget *waveWidgetO2 = new AGWaveWidget(WAVE_O2, "O2WaveWidget", AG_TYPE_O2);
    agParam.setWaveWidget(waveWidgetO2, AG_TYPE_O2);
    windowManager.addWidget(waveWidgetO2, trendWidgetO2);

    // TEMP部分。
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        paramManager.addProvider(*new TT3Provider());
        paramManager.addParam(tempParam.construction());
        alertor.addLimtSource(tempLimitAlarm.construction());
        alertor.addOneShotSource(tempOneShotAlarm.construction());
        TEMPTrendWidget *tempTrendWidget = new TEMPTrendWidget();
        tempParam.setTrendWidget(tempTrendWidget);
        windowManager.addWidget(tempTrendWidget);
    }

    // 关联设备和参数对象。
    paramManager.connectParamProvider();

    alertor.addOneShotSource(systemAlarm.Construction());
}

/**************************************************************************************************
 * 功能： 初始化打印。
 *************************************************************************************************/
static void _initPrint(void)
{
    // 初始化打印。
    printExec.construction();
    printManager.construction();
    alertor.addOneShotSource(printOneShotAlarm.construction());
    PRT72Provider *prtProvider = new PRT72Provider();
    printManager.setPrintProviderIFace(prtProvider);
    printManager.addPrintLayout(new PrintContinuousLayout());
    printManager.addPrintLayout(new PrintTrendDataLayout());
    printManager.addPrintLayout(new PrintTriggerSummaryLayout());
    printManager.addPrintLayout(new PrintViewSummaryLayout());
    printManager.addPrintLayout(new PrintECG12LeadLayout());
    printManager.addPrintLayout(new PrintOtherTriggerLayout());
    printManager.addPrintLayout(new PrintSupervisorCfgLayout());
    printManager.selftest();
    paramManager.addProvider(*prtProvider);

}

/**************************************************************************************************
 * 功能： 初始化菜单。
 *************************************************************************************************/
static void _initMenu(void)
{
    menuManager.construction();
    publicMenuManager.construction();
    publicMenuManager.addSubMenu(&ecgMenu);
    publicMenuManager.addSubMenu(&respMenu);
    if (systemManager.isSupport(CONFIG_AG))
    {
        publicMenuManager.addSubMenu(&agMenu);
    }
    if (systemManager.isSupport(CONFIG_IBP))
    {
        publicMenuManager.addSubMenu(&ibpMenu);
    }
    if (systemManager.isSupport(CONFIG_CO))
    {
        publicMenuManager.addSubMenu(&coMenu);
    }
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        publicMenuManager.addSubMenu(&spo2Menu);
    }
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        publicMenuManager.addSubMenu(&nibpMenu);
    }
    if (systemManager.isSupport(CONFIG_CO2))
    {
        publicMenuManager.addSubMenu(&co2Menu);
    }
    publicMenuManager.addSubMenu(&alarmLimitMenu);
    if(systemManager.isSupport(CONFIG_WIFI) && NetworkManager::isWifiTurnOn())
    {
        publicMenuManager.addSubMenu(&wifiProfileMenu);
    }

    publicMenuManager.addSubMenu(&systemMenu);
    publicMenuManager.addSubMenu(&supervisorEntrance);
    publicMenuManager.addSubMenu(&serviceEntrance);


    //supervisorMenuManager
    supervisorMenuManager.construction();

    supervisorGeneralSetMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorGeneralSetMenu);

    if (systemManager.isSupport(CONFIG_ECG12LEADS))
    {
        supervisor12LMenu.construction();
        supervisorMenuManager.addSubMenu(&supervisor12LMenu);
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        supervisorNIBPMenu.construction();
        supervisorMenuManager.addSubMenu(&supervisorNIBPMenu);
    }

    supervisorAlarmLimitMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorAlarmLimitMenu);

    supervisorAlarmMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorAlarmMenu);

    supervisorLocalMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorLocalMenu);

    supervisorDisplayMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorDisplayMenu);

    supervisorPrintMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorPrintMenu);

    supervisorTimeMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorTimeMenu);

    supervisorCodeMarker.construction();
    supervisorMenuManager.addSubMenu(&supervisorCodeMarker);

    if (systemManager.isSupport(CONFIG_WIFI))
    {
        supervisorWifiMenu.construction();
        supervisorMenuManager.addSubMenu(&supervisorWifiMenu);

        supervisorMailMenu.construction();
        supervisorMenuManager.addSubMenu(&supervisorMailMenu);
    }

    supervisorPortMenu.construction();
    supervisorMenuManager.addSubMenu(&supervisorPortMenu);

    supervisorMenuManager.addReturnMenu();

    //serviceMenuManager
    serviceMenuManager.construction();
    serviceMenuManager.addReturnMenu();


    //其它弹出菜单初始化
    patientManager.construction();
    ecg12LDataAcquire.construction();
    ecg12LDataMessNote.construction();
    ecg12LExportDataWidget.construction();
    ecg12LSnapShotReview.construction();

    trendDataWidget.construction();
    trendRescueRangePrint.construction();
    codeMarkerWidget.construction();
}

/**************************************************************************************************
 * 功能： 初始化所有。
 *************************************************************************************************/
void newObjects(void)
{
    _initSystem();
    _initWindowManager();
    _initComponents();
    _initProviderParam();
    _initPrint();
    _initMenu();
}

/**************************************************************************************************
 * 功能： 退出运行。
 *************************************************************************************************/
void deleteObjects(void)
{
    deleteWindowManager();
    deleteWaveWidgetSelectMenu();
    deletePublicMenuManager();
    deleteSupervisorMenuManager();
    deleteServiceMenuManager();
    deleteMenuManager();
    //deletePatientMenu();
    deleteParamManager();
    deleteParamInfo();
    deletePatientManager();
    deleteTimeManager();
    deleteTimeDate();
    deleteMachineConfig();
    deleteSystemConfig();
    deleteSuperConfig();
    deleteSuperRunConfig();
    deleteSystemTick();
    deleteSystemManager();
    deleteSystemBoardProvider();
    deletePowerManager();
    deleteKeyActionManager();

    deleteDataStorageDirManager();

    deletepMessageBox();

    deleteLightManager();
    deleteSoundManager();

    deleteAlarm();
    deleteAlarmIndicator();
    deleteAlarmStateMachine();

    deleteWaveformCache();
    deleteTrendCache();
    deleteParamDataStorageManager();
    deleteTrendDataWidget();
    deleteTrendRescueRangePrint();
    deleteRescueDataExportWidget();
    deleteRescueDataDeleteWidget();

    deleteCodeMarkerWidget();

    deletePrintManager();
    deletePrintExec();

    deleteSummaryStorageManager();
    deleteSummaryRescueDataWidget();
    deleteSummaryRescueRangePrint();

//    deleteNetworkManager();
    deleteSupervisorConfigManager();
    deleteRawDataCollection();
    deleteRawDataCollectionTxt();
    deleteUsbManager();
    deleteActivityLogManager();

    deleteecg12LDataAcquire();
    deleteecg12LDataMessNote();
    deleteecg12LExportDataWidget();
    deleteecg12LSnapShotReview();
    deleteecg12LDataPrint();
    deleteecg12LDataStorage();
    deleteecg12LeadManager();

    deletemailAddressWidget();
    deleteColorManager();
    deleteFontManager();
    deleteLanguageManager();
    deleteErrorCatch();
    deleteErrorLog();
}