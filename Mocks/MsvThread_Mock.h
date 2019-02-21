

#ifndef MARSTECH_THREAD_MOCK_H
#define MARSTECH_THREAD_MOCK_H


#include "..\IMsvThread.h"

#include <gmock\gmock.h>


class MsvThread_Mock:
	virtual public IMsvThread
{
public:
	MOCK_CONST_METHOD0(IsRunning, bool());
	MOCK_CONST_METHOD0(Notify, void());
	MOCK_METHOD1(StartThread, MsvErrorCode(int32_t));
	MOCK_METHOD0(StopThread, MsvErrorCode());
	MOCK_METHOD1(StopAndWaitForThreadStop, MsvErrorCode(int32_t));
	MOCK_METHOD1(WaitForThreadStop, MsvErrorCode(int32_t));
};


#endif // MARSTECH_THREAD_MOCK_H
