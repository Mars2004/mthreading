

#ifndef MARSTECH_WORKER_FACTORY_MOCK_H
#define MARSTECH_WORKER_FACTORY_MOCK_H


#include "../MsvWorker_Factory.h"

MSV_DISABLE_ALL_WARNINGS

#include <gmock\gmock.h>

MSV_ENABLE_WARNINGS


class MsvWorker_Factory_Mock:
	public MsvWorker_Factory
{
public:
	MOCK_CONST_METHOD1(GetIMsvTask, MSV_INTERFACE_POINTER(IMsvTask)(std::function<void()>&));
	MOCK_CONST_METHOD2(GetIMsvTask, MSV_INTERFACE_POINTER(IMsvTask)(std::function<void(void*)>&, void*));
};


#endif // MARSTECH_WORKER_FACTORY_MOCK_H
