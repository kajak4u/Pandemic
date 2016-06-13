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
