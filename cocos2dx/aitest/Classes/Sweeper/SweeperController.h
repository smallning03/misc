#ifndef SweeperController_h__
#define SweeperController_h__

#include <vector>
#include <map>

#include "BaseScene.h"

#include "utils/Fixed.h"

#include "GeneticAlgorithm.h"
#include "SweeperObject.h"
#include "MineObject.h"

class SweeperController : public BaseScene
{
public:
	~SweeperController();
	static SweeperController *instance();
	static void destroy();

	void start();
	void clear();

	void update(float dt);

private:
	SweeperController();
	static SweeperController *s_instance;
	bool init();
	void nextGeneration(float dt);

	FixedPoint randomFixedPosition();

private:
	cocos2d::Label *m_labelGeneration;

	int m_generationCurrentTicks;

	GeneticAlgorithm m_algorithm;

	std::set<MineObject *> m_mines;
	std::vector<SweeperObject *> m_sweepers;

};

#endif // SweeperController_h__