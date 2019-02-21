

#ifndef MARSTECH_THREADPOOL_FACTORY_MOCK_H
#define MARSTECH_THREADPOOL_FACTORY_MOCK_H


#include "../MsvThreadPool_Factory.h"

MSV_DISABLE_ALL_WARNINGS

#include <gmock\gmock.h>

MSV_ENABLE_WARNINGS


class MsvThreadPool_Factory_Mock:
	public MsvThreadPool_Factory
{
public:
	MOCK_CONST_METHOD1(GetIMsvTask, MSV_INTERFACE_POINTER(IMsvTask)(std::function<void()>&));
	MOCK_CONST_METHOD2(GetIMsvTask, MSV_INTERFACE_POINTER(IMsvTask)(std::function<void(void*)>&, void*));
	MOCK_CONST_METHOD3(GetIMsvUniqueWorker, MSV_INTERFACE_POINTER(IMsvUniqueWorker)(std::shared_ptr<std::condition_variable>, std::shared_ptr<std::mutex>, std::shared_ptr<uint64_t>));
};


#endif // MARSTECH_THREADPOOL_FACTORY_MOCK_H
