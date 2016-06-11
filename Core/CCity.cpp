#include "CCity.hpp"
#include "CBoard.hpp"
#include "CStringOption.h"
#include "CEnumOption.h"
#include "CDiseaseCube.hpp"
#include <QMouseEvent>
#include <QMessageBox>
#include <QBitmap>
#include "core.h"
#include "CResearchStation.hpp"
#include <QDebug>
#include "CPawn.hpp"

std::unique_ptr<QBitmap> CCity::cityMask = nullptr;

CCity::CCity(QWidget * parent) :
    CBoardItem(parent),
    researchStation(nullptr),
    type(UNKNOWN),
    pawnsInCity(0),
    logicObj(nullptr)
{
    if (isDesigner()) {
        connect(this, &CCity::rightButtonUp, this, &CCity::onRightBtnUp);
        connect(this, &CCity::leftButtonUp, this, &CCity::onLeftBtnUp);
        options += new CStringOption("City name", name);
        options += new CEnumOption<DiseaseType>("Disease type", DiseaseType_SL, type);
    }
    else {
        connect(this, &CCity::leftButtonUp, this, &CCity::showCityMenu);
    }
    if (cityMask == nullptr)
        cityMask = std::unique_ptr<QBitmap>(new QBitmap(QPixmap("img/cityMask.png").createMaskFromColor(Qt::transparent)));
    setMask(*cityMask);
    bottom = new CBoardItem(parent);
    connect(this, &CCity::scaled, bottom, &CBoardItem::scale);
    bottom->scaleTo(zoomFactor);
    bottom->setMask(*cityMask);
}

CCity::CCity(const CCity & other) :
    CBoardItem(other),
    type(other.type),
    diseaseCubes(other.diseaseCubes),
    researchStation(nullptr),
    connections(other.connections),
    logicObj(other.logicObj)
{
    for (CCity* city : connections)
        city->connections += this;
    if (other.researchStation != nullptr)
        buildResearchStation();
    for (COption* opt : other.options)
        options += opt->clone();
    for (auto iter = diseaseCubes.begin(); iter != diseaseCubes.end(); ++iter)
        for (int i = 0; i < iter.value().size(); ++i)
            addCube(iter.key());
    if (isDesigner()) {
        connect(this, &CCity::rightButtonUp, this, &CCity::onRightBtnUp);
        connect(this, &CCity::leftButtonUp, this, &CCity::onLeftBtnUp);
        options += new CStringOption("City name", name);
        options += new CEnumOption<DiseaseType>("Disease type", DiseaseType_SL, type);
    }
}

CCity::~CCity()
{
}

void CCity::onLeftBtnUp(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
        toggleHighlight();
}

void CCity::onRightBtnUp(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
        addCube(type);
    else
        toggleSelect();
}

void CCity::select()
{
    if (isDesigner()) {
        for (CCity *neighbour : connections) {
            neighbour->setProperty("highlighted", true);
            neighbour->update();
        }
    }
    bottom->select();
}

void CCity::unselect()
{
    if (isDesigner()) {
        for (CCity *neighbour : connections) {
            neighbour->setProperty("highlighted", false);
            neighbour->update();
        }
    }
    bottom->unselect();
}

void CCity::toggleHighlight()
{
    CBoardItem *selected = getSelected();
    if (selected == nullptr) {
        QMessageBox::critical(nullptr, "Invalid action", "Error - no city selected");
        return;
    }
    CCity *selCity = dynamic_cast<CCity*>(selected);
    if (selCity == nullptr) {
        QMessageBox::critical(nullptr, "Invalid action", QString("Error - you cannot bind ") + metaObject()->className() + " object with " + selected->metaObject()->className() + " object");
        return;
    }
    if (connections.contains(selCity)) {
        connections -= selCity;
        selCity->connections -= this;
        bottom->setProperty("highlighted", false);
    }
    else {
        connections += selCity;
        selCity->connections += this;
        bottom->setProperty("highlighted", true);
    }
    update();
}

void CCity::updateOptions()
{
    QString tooltip = "City: " + name;
    tooltip += "\nDisease: " + DiseaseType_SL[static_cast<int>(type)];
    tooltip += "\nItem id: " + QString::number(getId());
    setToolTip(tooltip);
}

void CCity::unload()
{
    for (CCity *neighbour : connections)
        neighbour->connections -= this;
    QObject::deleteLater();
}

void CCity::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    QString disType = ts.readLine();
    type = (DiseaseType) DiseaseType_SL.indexOf(disType);
    int connectionsCount = ts.readLine().toInt();
    for (int i = 0; i < connectionsCount; ++i) {
        int neighbourId;
        ts >> neighbourId;
        CCity *destination = dynamic_cast<CCity*>(container->findItemById(neighbourId));        
        if (destination) {
            connections += destination;
            destination->connections += this;
        }
    }
    ts.readLine();
    int diseaseCount = ts.readLine().toInt();
    for (int i = 0; i < diseaseCount; ++i) {
        QString disName = ts.readLine();
        DiseaseType disType = (DiseaseType) DiseaseType_SL.indexOf(disName);
        int count = ts.readLine().toInt();
        while (count-- > 0 && false)
            addCube(disType);
    }
    bottom->setParent(dynamic_cast<QWidget*>(parent()));
    bottom->stackUnder(this);
    bottom->setStandardPos(standardPos);
    bottom->setStandardSize(standardSize);
    updateOptions();
}

void CCity::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << DiseaseType_SL[(int)type] << endl;
    ts << connections.size() << endl;
    for (CCity *connection : connections)
        ts << connection->getId() << " ";
    ts << endl << diseaseCubes.size() << endl;
    for (auto iter = diseaseCubes.begin(); iter != diseaseCubes.end(); ++iter)
        ts << DiseaseType_SL[iter.key()] << endl << iter.value().size() << endl;
}

const QSet<CCity*> CCity::neighbours() const
{
    return connections;
}

int CCity::cubesOf(DiseaseType type) const
{
    return diseaseCubes[type].size();
}

void CCity::addCube(DiseaseType color)
{
    CDiseaseCube* newCube = CDiseaseCube::createIn(this, color);
    diseaseCubes[color] += newCube;
    for (int index = DiseaseType_SL.size() - 1; index > (int)color; --index)
        for (CDiseaseCube* cube : diseaseCubes[(DiseaseType)index])
            if (cube->isFirst())
                newCube->stackUnder(cube);
    if (researchStation != nullptr)
        researchStation->stackUnder(this);
}

void CCity::removeCube(DiseaseType type)
{
    diseaseCubes[type].back()->disappear();
    diseaseCubes[type].pop_back();
}

void CCity::buildResearchStation()
{
    researchStation = CResearchStation::createIn(this);
}

void CCity::removeResearchStation()
{
    if (researchStation == nullptr)
        throw "No research station to remove!";
    researchStation->disappear();
    researchStation = nullptr;
}

int CCity::pawnEnters(CPawn* pawn)
{
    pawnsInCity += pawn;
    return pawnsInCity.size();
}
void CCity::pawnEscapes(CPawn* escaping)
{
    if (!pawnsInCity.contains(escaping))
        return;
    CPawn** toDelete = nullptr;
    for (int i = 0; i < pawnsInCity.size();++i)
        if (pawnsInCity[i] == escaping)
            toDelete = &pawnsInCity[i];
        else if(toDelete != nullptr)
            pawnsInCity[i]->setStandardMiddleAnim(pawnsInCity[i]->getStandardMiddle() - CPoint(0, -12));
    if (toDelete)
        pawnsInCity.erase(toDelete);
}
void CCity::scale(double factor) {
    CBoardItem::scale(factor);
    if (researchStation != nullptr)
        researchStation->scale(factor);
    for (QVector<CDiseaseCube*>& disease : diseaseCubes)
        for (CDiseaseCube* elem : disease)
            elem->scale(factor);
}

City * CCity::toLogic() const
{
    return logicObj;
}

void CCity::bindLogic(City *object)
{
    logicObj = object;
}

QString CCity::createObjectName() const
{
    return QString("CCity %1").arg(name);
}

QString CCity::createObjectName(const QString & name)
{
    return QString("CCity %1").arg(name);
}

void CCity::showCityMenu(QMouseEvent* event)
{
    if (container->isCurrentCity(this)) {
        container->showCityMenu(CPoint(this->pos()) + this->size() / 2);
        event->accept();
    }
}

DiseaseType CCity::getColor() const
{
    return type;
}
