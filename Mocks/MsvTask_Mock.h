

#ifndef MARSTECH_TASK_MOCK_H
#define MARSTECH_TASK_MOCK_H


#include "..\IMsvTask.h"

MSV_DISABLE_ALL_WARNINGS

#include <gmock\gmock.h>

MSV_ENABLE_WARNINGS


class MsvTask_Mock:
	public IMsvTask
{
public:
	MOCK_METHOD0(Execute, void());
};


#endif // MARSTECH_TASK_MOCK_H
