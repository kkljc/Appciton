/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "ScreenLayoutModel.h"
#include "WindowManager.h"
#include "DemoProvider.h"
#include "ParamManager.h"
#include "ScreenLayoutDefine.h"
#include <QLinkedList>
#include <QVector>
#include <QByteArray>
#include "IConfig.h"
#include "ParamInfo.h"
#include "IBPParam.h"
#include "OrderedMap.h"

#define COLUMN_COUNT 6
#define ROW_COUNT 8
#define MAX_WAVE_ROW 6
#define WAVE_END_COLUMN 4

struct LayoutNode
{
    LayoutNode()
        : pos(0), span(1), waveId(WAVE_NONE), editable(true)
    {}

    QString name;
    int pos;
    int span;
    WaveformID waveId;
    bool editable;
};

enum ParamNodeSpan
{
    PARAM_SPAN_ONE = 1,
    PARAM_SPAN_TWO = 2,
};

struct ParamNodeDescription
{
    ParamNodeDescription() : paramSpan(PARAM_SPAN_ONE) {}
    ParamNodeDescription(const QString &displayName, ParamNodeSpan span)
        : displayName(displayName), paramSpan(span) {}

    QString displayName;
    ParamNodeSpan paramSpan;
};

typedef QLinkedList<LayoutNode *> LayoutRow;

class ScreenLayoutModelPrivate
{
public:
    ScreenLayoutModelPrivate()
        : demoProvider(NULL)
    {
        demoProvider = qobject_cast<DemoProvider *>(paramManager.getProvider("DemoProvider"));
    }

    ~ScreenLayoutModelPrivate()
    {
        clearAllLayoutNodes();
    }

    /**
     * @brief clearAllLayoutNodes clear all the layout nodes
     */
    void clearAllLayoutNodes()
    {
        QVector<LayoutRow *>::iterator iter;
        for (iter = layoutNodes.begin(); iter != layoutNodes.end(); ++iter)
        {
            qDeleteAll(**iter);
            (*iter)->clear();
        }
        qDeleteAll(layoutNodes);
        layoutNodes.clear();
    }

    /**
     * @brief loadLayoutFromConfig load layout config
     * @param config
     */
    void loadLayoutFromConfig(const QVariantMap &config)
    {
        // clear all the node first
        clearAllLayoutNodes();


        QVariantList layoutRows = config["LayoutRow"].toList();
        if (layoutRows.isEmpty())
        {
            // might has only one element
            QVariant tmp = config["LayoutRow"];
            if (tmp.isValid())
            {
                layoutRows.append(tmp.toMap());
            }
            else
            {
                return;
            }
        }
        QVariantList::ConstIterator iter;
        for (iter = layoutRows.constBegin(); iter != layoutRows.constEnd(); ++iter)
        {
            QVariantList nodes = iter->toMap().value("LayoutNode").toList();

            if (nodes.isEmpty())
            {
                // might has only one element
                QVariantMap nm = iter->toMap().value("LayoutNode").toMap();
                if (nm.isEmpty())
                {
                    continue;
                }
                nodes.append(nm);
            }

            LayoutRow *row = new LayoutRow();

            QVariantList::ConstIterator nodeIter;
            for (nodeIter = nodes.constBegin(); nodeIter != nodes.constEnd(); ++nodeIter)
            {
                QVariantMap nodeMap = nodeIter->toMap();
                int span = nodeMap["@span"].toInt();
                if (span == 0)
                {
                    span = 1;
                }
                int pos = nodeMap["@pos"].toInt();
                bool editable = nodeMap["@editable"].toBool();
                LayoutNode *node = new LayoutNode();
                node->name = nodeMap["@text"].toString();
                node->editable = editable;
                node->pos = pos;
                node->span = span;
                node->waveId = waveIDMaps.value(node->name, WAVE_NONE);

                if (row->isEmpty())
                {
                    row->append(node);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = row->begin();
                    for (; iter != row->end(); ++iter)
                    {
                        if ((*iter)->pos > node->pos)
                        {
                            break;
                        }
                    }

                    row->insert(iter, node);
                }
            }
            layoutNodes.append(row);
        }
    }

    /**
     * @brief insertNode insert one node into the row
     * @param r layout row
     * @param n layout node
     */
    void insertNode(LayoutRow *r, LayoutNode *n)
    {
        Q_ASSERT(r != NULL);
        Q_ASSERT(n != NULL);
        LayoutRow::Iterator iter = r->begin();
        for (; iter != r->end(); ++iter)
        {
            if ((*iter)->pos > n->pos)
            {
                break;
            }
        }

        r->insert(iter, n);
    }

    /**
     * @brief getLayoutMap get the layout map from the current layout info
     * @return
     */
    QVariantMap getLayoutMap() const
    {
        QVariantList layoutRows;
        QVector<LayoutRow *>::ConstIterator iter = layoutNodes.constBegin();
        for (; iter != layoutNodes.constEnd(); ++iter)
        {
            QVariantList nodes;

            LayoutRow::ConstIterator nodeIter = (*iter)->constBegin();
            for (; nodeIter != (*iter)->constEnd(); ++nodeIter)
            {
                LayoutNode *n = *nodeIter;
                QVariantMap m;
                m["@span"] = n->span;
                m["@pos"] = n->pos;
                m["@editable"] = n->editable ? 1 : 0;
                m["@text"] = n->name;
                nodes.append(m);
            }

            QVariantMap nm;
            nm["LayoutNode"] = nodes;
            layoutRows.append(nm);
        }

        QVariantMap rm;
        rm["LayoutRow"] = layoutRows;

        return rm;
    }

    /**
     * @brief getWaveData get the wave data from the wave cache, if the wave is not exist in the cache,
     *                    we will try to fetch the data from the demo provider
     * @param waveid
     * @return
     */
    QByteArray getWaveData(WaveformID waveid)
    {
        if (!waveCaches.contains(waveid))
        {
            QByteArray d;
            if (demoProvider)
            {
                d = demoProvider->getDemoWaveData(waveid);
            }
            waveCaches.insert(waveid, d);
        }
        return waveCaches[waveid];
    }

    /**
     * @brief fillWaveData fill the wave info if the @info has a valid waveid
     * @param info the info structure that need to fill wave infos
     */
    void fillWaveData(ScreenLayoutItemInfo &info)
    {
        if (info.waveid == WAVE_NONE || !demoProvider)
        {
            return;
        }

        info.waveContent = getWaveData(info.waveid);
        if (info.waveid >= WAVE_ECG_I && info.waveid <= WAVE_ECG_V6)
        {
            info.baseLine = demoProvider->getBaseLine();
            info.waveMaxValue = 255;
            info.waveMinValue = 0;
            info.sampleRate = demoProvider->getWaveformSample();
        }
        else if (info.waveid == WAVE_RESP)
        {
            info.waveMaxValue = demoProvider->maxRESPWaveValue();
            info.waveMinValue = demoProvider->minRESPWaveValue();
            info.baseLine = demoProvider->getRESPBaseLine();
            info.sampleRate = demoProvider->getRESPWaveformSample();
        }
        else if (info.waveid == WAVE_SPO2)
        {
            info.waveMaxValue = demoProvider->getSPO2MaxValue();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getSPO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid == WAVE_CO2)
        {
            info.waveMaxValue = demoProvider->getCO2MaxWaveform();
            info.drawSpeed = 6.25;
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getCO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid >= WAVE_N2O && info.waveid <= WAVE_O2)
        {
            info.waveMaxValue = demoProvider->getN2OMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getN2OBaseLine();
            info.sampleRate = demoProvider->getN2OWaveformSample();
        }
        else if (info.waveid >= WAVE_ART && info.waveid <= WAVE_AUXP2)
        {
            info.waveMaxValue = demoProvider->getIBPMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getIBPBaseLine();
            info.sampleRate = demoProvider->getIBPWaveformSample();
        }
    }


    void loadItemInfos()
    {
        // two ecg wave at most
        // TODO: find the proper ECG Wave
        waveIDMaps.insert("ECG1Wave", WAVE_ECG_II);
        waveIDMaps.insert("ECG2Wave", WAVE_ECG_I);

        waveIDMaps.insert("RESPWave", WAVE_RESP);
        waveIDMaps.insert("SPO2Wave", WAVE_SPO2);
        waveIDMaps.insert("CO2Wave", WAVE_CO2);

        // find proper IBP Wave base of the wave name
        waveIDMaps.insert("IBP1Wave", ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_1)));
        waveIDMaps.insert("IBP2Wave", ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_2)));

        waveIDMaps.insert("N2OWave", WAVE_N2O);
        waveIDMaps.insert("AA1Wave", WAVE_AA1);
        waveIDMaps.insert("AA2Wave", WAVE_AA2);
        waveIDMaps.insert("O2Wave", WAVE_O2);

        paramNodeDescriptions["ECG"] = ParamNodeDescription("ECG", PARAM_SPAN_TWO);
        paramNodeDescriptions["SPO2"] = ParamNodeDescription("SPO2", PARAM_SPAN_TWO);
        paramNodeDescriptions["RESP"] = ParamNodeDescription("RESP", PARAM_SPAN_TWO);
        // IBP's pressure name is identical to it's wave name
        paramNodeDescriptions["IBP1"] = ParamNodeDescription(paramInfo.getParamWaveformName(waveIDMaps["IBP1Wave"]), PARAM_SPAN_TWO);
        paramNodeDescriptions["IBP2"] = ParamNodeDescription(paramInfo.getParamWaveformName(waveIDMaps["IBP2Wave"]), PARAM_SPAN_TWO);
        paramNodeDescriptions["CO2"] = ParamNodeDescription("CO2", PARAM_SPAN_TWO);
        paramNodeDescriptions["NIBP"] = ParamNodeDescription("NIBP", PARAM_SPAN_TWO);
        paramNodeDescriptions["NIBPList"] = ParamNodeDescription(trs("NIBPList"), PARAM_SPAN_TWO);
        paramNodeDescriptions["TEMP"] = ParamNodeDescription("TEMP", PARAM_SPAN_TWO);
        paramNodeDescriptions["AA1"] = ParamNodeDescription("AA1",  PARAM_SPAN_TWO);
        paramNodeDescriptions["AA2"] = ParamNodeDescription("AA2", PARAM_SPAN_TWO);
        paramNodeDescriptions["N2O"] = ParamNodeDescription("N2O", PARAM_SPAN_TWO);
        paramNodeDescriptions["O2"] = ParamNodeDescription("O2", PARAM_SPAN_TWO);
        paramNodeDescriptions["ST"] = ParamNodeDescription("ST", PARAM_SPAN_TWO);
    }

    /**
     * @brief findNode find a layout node at the modelindex
     * @param index the index of the model
     * @return the layout node if exists, otherwise return NULL
     */
    LayoutNode *findNode(const QModelIndex &index)
    {
        if (!index.isValid() || index.row() >= layoutNodes.count() || index.column() >= COLUMN_COUNT)
        {
            return NULL;
        }

        LayoutRow *r = layoutNodes[index.row()];
        LayoutRow::ConstIterator iter = r->constBegin();
        for (; iter != r->end(); ++iter)
        {
            if ((*iter)->pos == index.column())
            {
                return *iter;
            }
        }

        return NULL;
    }

    /**
     * @brief getLayoutedWaveforms the the layouted waveforms that has been layout on the screen
     * @return the waveform name list
     */
    QStringList getLayoutedWaveforms() const
    {
        QStringList list;
        QVector<LayoutRow *>::ConstIterator riter;
        for (riter = layoutNodes.constBegin(); riter != layoutNodes.end(); ++riter)
        {
            LayoutRow::ConstIterator iter = (*riter)->constBegin();
            for (; iter != (*riter)->end(); ++iter)
            {
                if ((*iter)->waveId != WAVE_NONE)
                {
                    list.append((*iter)->name);
                }
            }
        }
        return list;
    }

    /**
     * @brief getUnlayoutedWaveforms get the waveforms haven't been layouted
     * @return a list of unlayouted waveforms
     */
    QStringList getUnlayoutedWaveforms() const
    {
        QStringList existWaveforms = getLayoutedWaveforms();
        QStringList avaliablesWaveforms = waveIDMaps.keys();
        QStringList unlayoutedWaveforms;
        QStringList::ConstIterator iter;
        for (iter = avaliablesWaveforms.constBegin(); iter != avaliablesWaveforms.constEnd(); ++iter)
        {
            if (existWaveforms.contains(*iter))
            {
                continue;
            }

            unlayoutedWaveforms.append(*iter);
        }
        return unlayoutedWaveforms;
    }

    /**
     * @brief getLayoutedParams get the layouted parameters
     * @return
     */
    QStringList getLayoutedParams() const
    {
        QStringList list;
        QVector<LayoutRow *>::ConstIterator riter;
        int row = 0;
        for (riter = layoutNodes.constBegin(); riter != layoutNodes.end(); ++riter)
        {
            int column = 0;
            LayoutRow::ConstIterator iter = (*riter)->constBegin();
            for (; iter != (*riter)->end(); ++iter)
            {
                if ((*iter)->waveId == WAVE_NONE && (row >= MAX_WAVE_ROW || column >= WAVE_END_COLUMN))
                {
                    list.append((*iter)->name);
                }
                column += (*iter)->span;
            }
            row += 1;
        }
        return list;
    }

    /**
     * @brief getUnlayoutedParams get a list of params that haven't been layouted
     * @return the parameter's name list
     */
    QStringList getUnlayoutedParams() const
    {
        QStringList existParams = getLayoutedParams();
        QStringList avaliableParams = paramNodeDescriptions.keys();
        QStringList unlayoutedParams;
        QStringList::ConstIterator iter;
        for (iter = avaliableParams.constBegin(); iter != avaliableParams.constEnd(); ++iter)
        {
            if (existParams.contains(*iter))
            {
                continue;
            }
            unlayoutedParams.append(*iter);
        }
        return unlayoutedParams;
    }

    /**
     * @brief itemInWaveRegion check wether the item is item the wave region
     * @param index
     * @return true if in the wave region, otherwise, return false
     */
    bool itemInWaveRegion(const QModelIndex &index)
    {
        if ((index.row() < MAX_WAVE_ROW) && (index.column() < WAVE_END_COLUMN))
        {
            return true;
        }
        return false;
    }

    DemoProvider *demoProvider;
    QMap<WaveformID, QByteArray> waveCaches;
    QVector<LayoutRow *> layoutNodes;
    OrderedMap<QString, WaveformID> waveIDMaps;
    OrderedMap<QString, ParamNodeDescription> paramNodeDescriptions;    // store data to describe param node
};

ScreenLayoutModel::ScreenLayoutModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new ScreenLayoutModelPrivate)
{
}

int ScreenLayoutModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int ScreenLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

bool ScreenLayoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case ReplaceRole:
    case InsertRole:
    {
        // replace the node, wave and param node
        QString nodeName = value.toString();
        if (!nodeName.isEmpty())
        {
            LayoutNode * node = d_ptr->findNode(index);
            if (node)
            {
                node->name = nodeName;
                node->waveId = d_ptr->waveIDMaps.value(nodeName, WAVE_NONE); // for param node or empty node name, the id will always be None
            }

            QModelIndex anotherChangeIndex = index;

            // handle the row span for param node region
            if (!d_ptr->itemInWaveRegion(index))
            {
                ParamNodeDescription desc =  d_ptr->paramNodeDescriptions[nodeName];
                LayoutRow *r = d_ptr->layoutNodes.at(index.row());
                if (desc.paramSpan > node->span)
                {
                    // when we get here, we need more space to hold the param node
                    QModelIndex nextNodeIndex = this->index(index.row(), index.column() + node->span);
                    LayoutNode * nextNode = d_ptr->findNode(nextNodeIndex);
                    if (nextNode)
                    {
                        // just remove the node and the take the removed node's space
                        r->removeOne(nextNode);
                        node->span += nextNode->span;
                        delete nextNode;
                        emit spanChanged(index);
                    }
                    anotherChangeIndex = nextNodeIndex;
                }
                else if (desc.paramSpan < node->span)
                {
                    // too more space to hold the param node
                    int left = node->span - desc.paramSpan;
                    node->span = desc.paramSpan;

                    // create one more node to hold the space
                    LayoutNode *newNode  = new LayoutNode;
                    newNode->span = left;
                    newNode->pos = node->pos + node->span;
                    d_ptr->insertNode(r, newNode);
                    emit spanChanged(index);
                    anotherChangeIndex = this->index(index.row(), index.column() + node->span);
                }
            }

            emit dataChanged(index, anotherChangeIndex);
        }
    }
        break;
    case RemoveRole:
    {
        // remove the current node
        LayoutNode * node = d_ptr->findNode(index);
        if (node)
        {
            node->waveId = WAVE_NONE;
            node->name = QString();
            QModelIndex anotherIndex = index;

            // handle the row span for param node region
            if (!d_ptr->itemInWaveRegion(index))
            {
                if (node->span == PARAM_SPAN_TWO)
                {
                    node->span = PARAM_SPAN_ONE;
                    // sperate all the node space into span of one
                    LayoutRow *r = d_ptr->layoutNodes.at(index.row());
                    // just add a node with span of one
                    LayoutNode *newNode = new LayoutNode();
                    newNode->pos = node->pos + node->span;
                    d_ptr->insertNode(r, newNode);
                    anotherIndex = this->index(index.row(), newNode->pos);
                    emit spanChanged(index);
                }
            }
            emit dataChanged(index, anotherIndex);
        }
    }
        break;
    default:
        break;
    }

    return false;
}

QVariant ScreenLayoutModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            ScreenLayoutItemInfo info;
            info.waveid = node->waveId;
            info.name = node->name;
            if (node->waveId != WAVE_NONE)
            {
                info.displayName = paramInfo.getParamWaveformName(node->waveId);
            }
            else
            {
                // param node
                ParamNodeDescription desc = d_ptr->paramNodeDescriptions.value(info.name);
                if (!desc.displayName.isEmpty())
                {
                    info.displayName = desc.displayName;
                }
                else
                {
                    info.displayName = info.name;
                }
            }

            d_ptr->fillWaveData(info);
            return qVariantFromValue(info);
        }
    }
    break;
    case ReplaceRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            if (node->name.isEmpty())
            {
                // can't replace, the node is already empty
                break;
            }
        }
        else
        {
            break;
        }

        if (d_ptr->itemInWaveRegion(index))
        {
            QStringList replaceWaveforms = d_ptr->getUnlayoutedWaveforms();

            // no avaliable waveforms, so no replaceable waveforms
            if (replaceWaveforms.isEmpty())
            {
                break;
            }

            // also add the current edit wave
            replaceWaveforms.append(node->name);

            QVariantList list;
            QStringList::ConstIterator iter = replaceWaveforms.constBegin();
            for (; iter != replaceWaveforms.constEnd(); ++iter)
            {
                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = paramInfo.getParamWaveformName(d_ptr->waveIDMaps.value(*iter, WAVE_NONE));
                list.append(m);
            }

            return list;
        }
        else
        {
            QStringList replaceParams = d_ptr->getUnlayoutedParams();

            // no avaliable params, so no replaceable waveforms
            if (replaceParams.isEmpty())
            {
                break;
            }

            // also add the current edit param
            replaceParams.append(node->name);


            QVariantList list;
            QStringList::ConstIterator iter = replaceParams.constBegin();
            bool oddColumn = index.column() % 2;  // this column can only hold param node with span of 1
            for (; iter != replaceParams.constEnd(); ++iter)
            {
                if (oddColumn && d_ptr->paramNodeDescriptions[*iter].paramSpan == PARAM_SPAN_TWO)
                {
                    // the node can't hold the param node
                    continue;
                }

                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = d_ptr->paramNodeDescriptions[*iter].displayName;
                list.append(m);
            }

            return list;
        }
    }

    case InsertRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            if (!node->name.isEmpty())
            {
                // can't insert, the node is not empty
                break;
            }
        }

        if (d_ptr->itemInWaveRegion(index))
        {

            QVariantList list;
            QStringList unlayoutWaves = d_ptr->getUnlayoutedWaveforms();
            QStringList::ConstIterator iter = unlayoutWaves.constBegin();
            for (; iter != unlayoutWaves.constEnd(); ++iter)
            {
                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = paramInfo.getParamWaveformName(d_ptr->waveIDMaps.value(*iter, WAVE_NONE));
                list.append(m);
            }

            return list;
        }
        else
        {
            QVariantList list;
            QStringList unlayoutParams = d_ptr->getUnlayoutedParams();
            QStringList::ConstIterator iter = unlayoutParams.constBegin();
            bool oddColumn = index.column() % 2; // this column can only hold param node with span of 1
            for (; iter != unlayoutParams.constEnd(); ++iter)
            {
                if (oddColumn && d_ptr->paramNodeDescriptions[*iter].paramSpan == PARAM_SPAN_TWO)
                {
                    // the node can't hold the param node
                    continue;
                }

                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = d_ptr->paramNodeDescriptions[*iter].displayName;
                list.append(m);
            }
            return list;
        }
    }
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ScreenLayoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    LayoutNode *node = d_ptr->findNode(index);
    bool editable = false;
    if (node)
    {
        editable = node->editable;
    }

    if (editable)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::NoItemFlags;
    }
}

QSize ScreenLayoutModel::span(const QModelIndex &index) const
{
    if (index.isValid())
    {
        LayoutNode *node = d_ptr->findNode(index);
        // if the node exist, use the node's span value, otherwise, use the default value
        if (node)
        {
            return QSize(node->span, 1);
        }
    }

    return QSize();
}

void ScreenLayoutModel::saveLayoutInfo()
{
    systemConfig.setConfig("PrimaryCfg|UILayout|ScreenLayout|Normal", d_ptr->getLayoutMap());
}

void ScreenLayoutModel::loadLayoutInfo()
{
    const QVariantMap config = systemConfig.getConfig("PrimaryCfg|UILayout|ScreenLayout|Normal");
    beginResetModel();
    d_ptr->loadLayoutFromConfig(config);
    endResetModel();

    // load the item's row span
    int row = rowCount();
    int column = columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            QModelIndex idx = index(i, j);
            if (d_ptr->findNode(idx))
            {
                emit spanChanged(idx);
            }
        }
    }
}

void ScreenLayoutModel::updateWaveAndParamInfo()
{
    d_ptr->loadItemInfos();
}

ScreenLayoutModel::~ScreenLayoutModel()
{
    delete d_ptr;
}