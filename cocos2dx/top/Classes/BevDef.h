#ifndef BevDef_h__
#define BevDef_h__

enum class BevRunningStatus
{
	Executing = 0,
	Finish = 1,
	Terminal = -1, // �ڵ���executing��transition
};

#define BevInvalidChildNodeIndex -1

#endif // BevDef_h__