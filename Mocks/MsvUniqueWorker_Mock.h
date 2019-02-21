

#ifndef MARSTECH_UNIQUEWORKER_MOCK_H
#define MARSTECH_UNIQUEWORKER_MOCK_H


#include "..\IMsvUniqueWorker.h"
#include "MsvThread_Mock.h"

#include <gmock\gmock.h>


class MsvUniqueWorker_Mock:
	public IMsvUniqueWorker,
	public MsvThread_Mock
{
public:
	MOCK_METHOD1(SetTask, MsvErrorCode(std::shared_ptr<IMsvTask>));
	MOCK_METHOD1(SetTask, MsvErrorCode(std::function<void()>&));
	MOCK_METHOD2(SetTask, MsvErrorCode(std::function<void(void*)>&, void*));
};


#endif // MARSTECH_UNIQUEWORKER_MOCK_H
