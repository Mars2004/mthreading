

#ifndef MARSTECH_THREADPOOL_MOCK_H
#define MARSTECH_THREADPOOL_MOCK_H


#include "..\IMsvThreadPool.h"

#include <gmock\gmock.h>


class MsvThreadPool_Mock:
	public IMsvThreadPool
{
public:
	MOCK_METHOD1(AddTask, void(std::shared_ptr<IMsvTask>));
	MOCK_METHOD1(AddTask, MsvErrorCode(std::function<void()>&));
	MOCK_METHOD2(AddTask, MsvErrorCode(std::function<void(void*)>&, void*));
	MOCK_CONST_METHOD0(IsRunning, bool());
	MOCK_METHOD1(StartThreadPool, MsvErrorCode(uint16_t));
	MOCK_METHOD0(StopThreadPool, MsvErrorCode());
	MOCK_METHOD1(StopAndWaitForThreadPoolStop, MsvErrorCode(int32_t));
	MOCK_METHOD1(WaitForThreadPoolStop, MsvErrorCode(int32_t));
};


#endif // MARSTECH_THREADPOOL_MOCK_H
