

#ifndef MARSTECH_WORKER_MOCK_H
#define MARSTECH_WORKER_MOCK_H


#include "..\IMsvWorker.h"
#include "MsvThread_Mock.h"

#include <gmock\gmock.h>


class MsvUniqueWorker_Mock:
	public IMsvWorker,
	public MsvThread_Mock
{
public:
	MOCK_METHOD1(AddTask, void(std::shared_ptr<IMsvTask>));
	MOCK_METHOD1(AddTask, MsvErrorCode(std::function<void()>&));
	MOCK_METHOD2(AddTask, MsvErrorCode(std::function<void(void*)>&, void*));
};


#endif // MARSTECH_WORKER_MOCK_H
