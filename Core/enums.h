#pragma once
#include <QStringList>

enum DiseaseType {
    UNKNOWN = 0,
    BLUE = 1,
    YELLOW = 2,
    BLACK = 3,
    RED = 4
};
const QStringList DiseaseType_SL = { "<NULL>", "BLUE", "YELLOW", "BLACK", "RED" };

enum Insertable {
    CLASS_City = 0,
    CLASS_CureMarker,
    CLASS_PathMarker,
    CLASS_PathItem,
    CLASS_Deck,
    CLASS_Pawn,
    CLASS_DiseaseCube,
    CLASS_ResearchStation,
    CLASS_Card
};
const QStringList Insertable_SL = {"CCity", "CCureMarker", "CPathMarker", "CPathItem", "CDeck", "CPawn", "CDiseaseCube", "CResearchStation", "CCard"};

enum CardType {
    CT_UNKNOWN = 0,
    CT_DISEASE,
    CT_PLAYER,
    CT_SPECIAL,
    CT_EPIDEMIC
};
const QStringList CardType_SL = {"Unknown", "Disease", "Player", "Special", "Epidemic"};

enum DeckType {
    DT_PLAYER,
    DT_PLAYERDISCARD,
    DT_DISEASE,
    DT_DISEASEDISCARD
};
const QStringList DeckType_SL = {"PlayerDeck", "PlayerDiscard", "DiseaseDeck", "DiseaseDiscard"};

enum CureStatus {
    NEW = 0,
    DISCOVERED = 1,
    TREATED = 2
};
const QStringList CureStatus_SL = {"New", "Discovered", "Treated"};

enum PathType {
    PATH_NONE, 
    PATH_OUTBREAKS,
    PATH_INFECTIONS
};
static const QStringList PathType_SL = { "None", "Outbreaks path", "Infections path" };

enum PlayerRole {
    ROLE_RANDOM = 0,
    ROLE_DISPATCHER,
    ROLE_MEDIC,
    ROLE_OPERATIONSEXPERT,
    ROLE_RESEARCHER,
    ROLE_SCIENTIST
};
enum Difficulty {
    DIFF_INTRODUCTORY = 0,
    DIFF_MEDIUM,
    DIFF_HEROIC
};
static const QStringList Difficulty_SL = {"Introductory", "Medium", "Heroic"};

static const QStringList PlayerRole_SL = { "[Random]", "Dispatcher", "Medic", "Operations Expert", "Researcher", "Scientist" };

static const QStringList RoleDescriptions_SL = {
    "<html><head/><body><table><tr><td><img src=\":/roles/img/random.png\"           width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>You get a random role.</li></ul></td></tr></table></body></html>",
    "<html><head/><body><table><tr><td><img src=\":/roles/img/dispatcher.png\"       width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>Move your fellow's pawns on your turn as if they were your own.</li><li>Move a pawn to another city containing a pawn for 1 action.</li></ul></td></tr></table></body></html>",
    "<html><head/><body><table><tr><td><img src=\":/roles/img/medic.png\"            width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>Remove all cubes of a single color when you treat a city.</li><li>Administer known cures for free.</li></ul></td></tr></table></body></html>",
    "<html><head/><body><table><tr><td><img src=\":/roles/img/operations expert.png\" width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>You may build a research station in your current city for one action.</li></ul></td></tr></table></body></html>",
    "<html><head/><body><table><tr><td><img src=\":/roles/img/researcher.png\"       width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>You may give a player cards from your hand for 1 action per card.</li><li>Both of your pawns must be in the same city, but it doesn't matter which city you are in</li></ul></td></tr></table></body></html>",
    "<html><head/><body><table><tr><td><img src=\":/roles/img/scientist.png\"        width=100 height=100 /></td><td style=\"vertical-align: middle;\"><ul><li>You need only 4 cards of the same color to discover a cure.</li></ul></td></tr></table></body></html>"
};

//decyzja dotycz¹ca ruchu gracza

enum Decision
{
    DEC_MOVE_TO_PLAYER = -2,		//j.n.
    DEC_MOVE_ANOTHER = -1,			//dispatcher
    DEC_MOVE_SHORT = 0,
    DEC_MOVE_TO_CARD = 1,
    DEC_MOVE_EVERYWHERE,
    //DEC_MOVE_TO_STATION,
    DEC_BUILD_STATION,
    DEC_DISCOVER_CURE,
    DEC_TREAT,
    DEC_GAIN_CARD,
    DEC_GIVE_CARD,
    DEC_USE_SPECIAL,
    DEC_PASS
};

enum SpecialCardType //wartosci dla pewnosci w petli dodawania
{
    SC_ONE_QUIET_NIGHT = 0,
    SC_FORECAST = 1,
    SC_RESILIENT_POPULATION = 2,
    SC_AIRLIFT = 3,
    SC_GOVERNMENT_GRANT = 4
};
static const QStringList SpecialCardType_SL = { "One Quiet Night", "Forecast", "Resilient Population","Airlift","Government Grant" };

enum GameResult
{
	LOST_OUTBREAKS = -3,
	LOST_CUBES = -2, 
	LOST_CARDS = -1,
    IN_PROGRESS = 0,
    WON = 1
};					 