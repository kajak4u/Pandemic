#include "City.h"
#include "Board.h" //dla uzycia Outbreak()

//void City::ClearVisitedList()
//{
//	visitedInSpread.clear();
//}

int City::DiseaseCounter(DiseaseType i) //const
{
	return diseaseCubes[i];
}

DiseaseType City::GetColor() const
{
	return type;
}

void City::AddCube(DiseaseType diseaseID)
{
	diseaseCubes[diseaseID]++;
}

//void City::Spread(Disease* disease) // nalozyc sprawdzanie - na zewnatrz: czy choroba jest HEALED
//{
//	bool visited = false;
//	for (City* visitCity : visitedInSpread)	
//	{
//		if (this->name == visitCity->name)
//		{
//			visited = true;
//		}
//	}
//	if (!visited)
//	{
//		visited = true;
//		visitedInSpread.push_back(this);
//		if (diseaseCubes[disease->getID()] == 3)
//		{
//			Board::Outbreak();		// outbreaksMarker++
//			for each (City* city in neighbourhood)
//			{
//				city->Spread(disease);
//			}
//		}
//		else
//		{
//			if (disease->MarkersLeft() > 0)
//			{
//				diseaseCubes[disease->getID()]++;
//				disease->PutMarker();
//						cout << endl << name << " - COLOR:  " << DiseaseType_SL[disease->getID()].toStdString() << " - COUNTER: " << diseaseCubes[disease->getID()] << endl;
//			}
//			else
//			{
//				throw string(disease->getColor().toStdString() + " - no markers left");				
//			}
//		}
//	}
//}

void City::RemoveCube(DiseaseType diseaseID)
{
	diseaseCubes[diseaseID]--;
			cout << endl << name << " - COLOR:  " << DiseaseType_SL[diseaseID].toStdString() << " - COUNTER: " << diseaseCubes[diseaseID] << endl;
}
void City::RemoveAllCubes(DiseaseType diseaseID)
{
	diseaseCubes[diseaseID] = 0;
			cout << endl << name << " - COLOR:  " << DiseaseType_SL[diseaseID].toStdString() << " - COUNTER: " << diseaseCubes[diseaseID] << endl;
}

//void City::HealOne(Disease* disease) //nalozyc sprawdzanie przy wywolaniu !!!
//{
//	disease->RemoveMarkers(diseaseCubes[disease->getID()]);
//	diseaseCubes[disease->getID()]--;	
//		cout << endl << name << " - COLOR:  " << DiseaseType_SL[disease->getID()].toStdString() << " - COUNTER: " << diseaseCubes[disease->getID()] << endl;
//}
//									
//void City::HealAll(Disease* disease)		  
//{
//	disease->RemoveMarkers(diseaseCubes[disease->getID()]);
//	diseaseCubes[disease->getID()] = 0;
//		cout << endl << name << " - COLOR:  " << DiseaseType_SL[disease->getID()].toStdString() << " - COUNTER: " << diseaseCubes[disease->getID()] << endl;
//}

bool City::IsStation() const
{
	return hasResearchStation;
}

vector<City*> City::Neighbours() const
{
	return neighbourhood;
}

void City::BuildResearchStation()
{
	hasResearchStation = true;
}

void City::RemoveResearchStation()
{
	hasResearchStation = false;
}

vector<City*> City::AddNeighbours(const map<City*, vector<string>>& citiesWithNeighbourhood)
{
	vector<City*> allCities;
	for (pair<City*, vector<string>> oneCity : citiesWithNeighbourhood)
	{
		allCities.push_back(oneCity.first);
	}
	for (pair<City*, vector<string>> oneCity : citiesWithNeighbourhood)
	{
		for (string name : oneCity.second)	   //iteruj po nazwach sasiadow
		{
			for (City* city : allCities)		  //dla kazdej nazwy iteruj po miastach
			{
				if (city->name == name)		   //..i szukaj tego wlasciwego
				{
					oneCity.first->neighbourhood.push_back(city);	  //..ktore dodasz jako sasiada (wskaz)
					break;
				}
			}
		}
	}
	return allCities;
}

string City::GetName() const
{
	return name;
}

City::City()
{
}

City::City(string cityName, DiseaseType color) :name(cityName), type(color)
{
	hasResearchStation = false;
}


City::~City()
{
}
