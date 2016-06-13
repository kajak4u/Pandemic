#include "Disease.h"

#include "Board.h"
#include <qset.h>
#include "Mediator.h"
#include <iostream>
using namespace std;

void Disease::Discover()
{
	status = DISCOVERED;
	if (markersLeft == 24)
	{
		Eradicate();
	}
	mediator().setDiseaseStatus(diseaseID, status);
}

void Disease::Eradicate()
{
	status = TREATED;
}

void Disease::RemoveMarkers(int count)
{
	markersLeft += count;
}

void Disease::PutMarker()
{
	--markersLeft;
}

int Disease::MarkersLeft() const
{
	return markersLeft;
}

QString Disease::getColor() const
{
	return DiseaseType_SL[diseaseID];
}

DiseaseType Disease::getID() const
{
	return diseaseID;
}

CureStatus Disease::Status() const
{
	return status;
}

Disease::Disease(QString color)
{
	diseaseID = (DiseaseType)DiseaseType_SL.indexOf(color);
	markersLeft = 24;
	status = NEW;
}		
//enum DiseaseType
//{
//	UNKNOWN = 0,
//	BLUE = 1,
//	YELLOW = 2,
//	BLACK = 3,
//	RED = 4
//};

Disease::~Disease()
{
}
//==============================================================================================================================================


void Disease::addSingleCube(City* city)
{
	if (status == TREATED)
		return;
	if (city->DiseaseCounter(diseaseID) < 3)	  //krytyczne sprawdzanie
	{
		city->AddCube(diseaseID);
		--markersLeft;
		mediator().addDiseaseCube(city, diseaseID, 1);
	}
	else
		throw exception("PONAD 3 KOSTKI W MIESCIE !!!");	//critical error
}

//dodawanie kostki, ewentualnie wybuch; zwraca liczbê wybuchów
void Disease::diseaseAppearsIn(City *source, int count)
{
	if (markersLeft - count < 0)
	{
		Board::WinOrLoose(LOST_CUBES);
	}
	
	QSet<City*> processed = { source };
	QSet<City*> toProcess;
	if (source->DiseaseCounter(diseaseID) + count <= 3)
	{ //nie bêdzie outbreaka
		for (int i = 0; i < count; ++i)
		{
			addSingleCube(source);
		}
	}
	else
	{  //outbreak
		for (int i = source->DiseaseCounter(diseaseID); i<3; ++i)
			addSingleCube(source);
		Board::Outbreak();
		for (City *neighbour : source->Neighbours())
			toProcess += neighbour;
		while (!toProcess.empty())
		{
			City *actual = *toProcess.begin();
			toProcess -= actual;
			processed += actual;
			if (actual->DiseaseCounter(diseaseID) == 3)
			{
				Board::Outbreak(); //mozna zlapac tutaj i dodac info, ktore poleca wyzej
				for (City *neighbour : actual->Neighbours())
					if (!processed.contains(neighbour))
						toProcess += neighbour;
			}
			else
				addSingleCube(actual);
		}
	}
	return;
}

void Disease::healDisease(City * location, PlayerRole role)	   //nalozyc WYZEJ sprawdzenie, czy mozna wywolac...	||wywolac w przemieszczaniu medyka po wynalezieniu choroby
{
	Mediator& m = mediator();
	if (status == NEW && role != ROLE_MEDIC)
	{
		location->RemoveCube(diseaseID);
		++markersLeft;
		m.removeCube(location, diseaseID);
	}
	else	 //status == DISCOVERED || role == ROLE_MEDIC	   
	{
		int number = location->DiseaseCounter(diseaseID);
		markersLeft += number;
		location->RemoveAllCubes(diseaseID);
		mediator().removeCube(location, diseaseID, number);
	}
	if (status == DISCOVERED && markersLeft == 24)
	{
		Eradicate();
		m.setDiseaseStatus(diseaseID, status);
	}
}