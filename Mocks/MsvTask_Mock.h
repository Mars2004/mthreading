

#ifndef MARSTECH_TASK_MOCK_H
#define MARSTECH_TASK_MOCK_H


#include "..\IMsvTask.h"

#include <gmock\gmock.h>


class MsvTask_Mock:
	public IMsvTask
{
public:
	MOCK_METHOD0(Execute, void());
};


#endif // MARSTECH_TASK_MOCK_H
