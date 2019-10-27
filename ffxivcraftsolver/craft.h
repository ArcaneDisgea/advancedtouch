#pragma once
#include <string>
#include <map>
#include "common.h"
#include "random.h"

enum class actions
{
	// Synthesis
	basicSynth,
	carefulSynthesis,
	rapidSynthesis,
	focusedSynthesis,
	delicateSynthesis,
	intensiveSynthesis,
	muscleMemory,
	brandOfTheElements,

	// Touch
	basicTouch,
	standardTouch,
	hastyTouch,
	byregotsBlessing,
	preciseTouch,
	focusedTouch,
	patientTouch,
	prudentTouch,
	preparatoryTouch,
	trainedEye,

	// CP
	tricksOfTheTrade,

	// Durability
	mastersMend,
	wasteNot,
	manipulation,

	// Buffs
	innerQuiet,
	reflect,
	ingenuity,
	greatStrides,
	innovation,
	nameOfTheElements,
	finalAppraisal,

	observe,
	reuse
};

const std::map<actions, std::string> simpleText = {
	{actions::basicSynth, "basicSynth"},
	{actions::carefulSynthesis, "carefulSynthesis"},
	{actions::rapidSynthesis, "rapidSynthesis"},
	{actions::focusedSynthesis, "focusedSynthesis"},
	{actions::delicateSynthesis, "delicateSynthesis"},
	{actions::intensiveSynthesis, "intensiveSynthesis"},
	{actions::muscleMemory, "muscleMemory"},
	{actions::brandOfTheElements, "brandOfTheElements"},

	{actions::basicTouch, "basicTouch"},
	{actions::standardTouch, "standardTouch"},
	{actions::hastyTouch, "hastyTouch"},
	{actions::byregotsBlessing, "byregotsBlessing"},
	{actions::preciseTouch, "preciseTouch"},
	{actions::focusedTouch, "focusedTouch"},
	{actions::patientTouch, "patientTouch"},
	{actions::prudentTouch, "prudentTouch"},
	{actions::preparatoryTouch, "preparatoryTouch"},
	{actions::trainedEye, "trainedEye"},

	{actions::tricksOfTheTrade, "tricksOfTheTrade"},

	{actions::mastersMend, "mastersMend"},
	{actions::wasteNot, "wasteNot"},
	{actions::manipulation, "manipulation"},

	{actions::innerQuiet, "innerQuiet"},
	{actions::reflect, "reflect"},
	{actions::ingenuity, "ingenuity"},
	{actions::greatStrides, "greatStrides"},
	{actions::innovation, "innovation"},
	{actions::nameOfTheElements, "nameOfTheElements"},
	{actions::finalAppraisal, "finalAppraisal"},

	{actions::observe, "observe"},
	{actions::reuse, "reuse"}
};


class craft
{
public:
	using sequenceType = std::vector<actions>;

	struct endResult
	{
		int progress;
		int quality;				/// not in union to allow solver tiebreaks
		union
		{
			int hqPercent;			// normal and hqOrBust mode
			bool collectableHit;	// collectable mode
		};
		int durability;
		int steps;
		int invalidActions;
		bool reuseUsed;
	};

	enum class condition
	{
		poor,
		normal,
		good,
		excellent
	};

	enum class actionResult
	{
		success,
		failRNG,
		failNoCP,
		failHardUnavailable,	// for things that should count towards the unavailable counter
		failSoftUnavailable		// for things that might randomly proc
	};

private:
	const crafterStats crafter;
	const recipeStats recipe;

	int step;
	int durability; 
	int CP;
	int quality;
	int progress;

	condition cond;

	int progressFactor;
	int qualityFactor;

	// Buffs
	int muscleMemoryTime = 0;
	int wasteNotTime = 0;
	int manipulationTime = 0;
	int innerQuietStacks = 0;
	int ingenuityTime = 0;
	int greatStridesTime = 0;
	int innovationTime = 0;
	int nameOfTheElementsTime = 0;
	bool nameOfTheElementsUsed = false;
	int finalAppraisalTime = 0;
	bool observeCombo = false;	// For Focused combo

	const bool normalLock;
	bool reuseActive = false;

	random& rng;

	// chance == 70 means 70% success and so on
	inline bool rollPercent(int chance) const;

	void calculateFactors();

	void increaseProgress(int efficiency);
	void increaseQuality(int efficiency);

	// Negative amount to deduct, positive to add. Returns false if not enough CP (will not deduct if so)
	bool changeCP(int amount);

	// Negative to remove durability, positive to add.
	void changeDurability(int amount)
	{
		durability = std::min(durability + amount, recipe.durability);
	}

	void hitDurability(int amount = 10)
	{
		return changeDurability(wasteNotTime > 0 ? -amount / 2 : -amount);
	}

	condition getNextCondition(condition current);
	void endStep();

	void nameOfTheElementsPost();

	// both percentages from 0-100
	static int hqPercentFromQuality(int qualityPercent);

public:
	craft() = delete;
	craft(int initialQuality, crafterStats cS, recipeStats rS, bool nLock, random& r) :
		crafter(cS),
		recipe(rS),
		step(1),
		durability(recipe.durability),
		CP(crafter.CP),
		quality(initialQuality),
		progress(0),
		cond(condition::normal),
		normalLock(nLock),
		rng(r)
	{
		calculateFactors();
	}

	bool checkBuffsValid() const;
	std::string getState() const;

private:
	actionResult commonSynth(int cpChange, int efficiency, int successChance, bool doubleDurability = false);
	actionResult commonTouch(int cpChange, int efficiency, int successChance, bool doubeDurability = false);

	// Synthesis
	actionResult basicSynth() { return commonSynth(0, crafter.level >= 31 ? 120: 100, 100); }
	actionResult carefulSynthesis() { return commonSynth(-7, 150, 100); }
	actionResult rapidSynthesis() { return commonSynth(0, crafter.level >= 63 ? 500 : 250, 50); }
	actionResult focusedSynthesis() { return commonSynth(-5, 200, observeCombo ? 100 : 50); }
	actionResult delicateSynthesis();
	actionResult intensiveSynthesis();
	actionResult muscleMemory();
	void muscleMemoryPost();
	actionResult brandOfTheElements();

	// Touches
	actionResult basicTouch() { return commonTouch(-18, 100, 100); }
	actionResult standardTouch() { return commonTouch(-32, 125, 100); }
	actionResult hastyTouch() { return commonTouch(0, 100, 60); }
	actionResult byregotsBlessing();
	actionResult preciseTouch();
	actionResult focusedTouch() { return commonTouch(-18, 150, observeCombo ? 100 : 50); }
	actionResult patientTouch();
	actionResult prudentTouch();
	actionResult preparatoryTouch();
	actionResult trainedEye();

	// CP
	actionResult tricksOfTheTrade();

	// Durability
	actionResult mastersMend();
	actionResult wasteNot();
	void wasteNotPost();
	actionResult manipulation();
	void manipulationPost();

	// Buffs
	actionResult innerQuiet();
	actionResult reflect();
	actionResult ingenuity();
	void ingenuityPost();
	actionResult greatStrides();
	void greatStridesPost();
	actionResult innovation();
	void innovationPost();
	actionResult nameOfTheElements();
	actionResult finalAppraisal();
	void finalAppraisalPost();


	actionResult observe();
	void observePost();
	actionResult reuse();

public:
	actionResult performOne(actions action);
	void performOnePost(actions action);
	endResult performAll(const sequenceType& sequence, goalType goal, int progessWiggle, bool echoEach = false);
};
