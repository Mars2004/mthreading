

#ifndef MARSTECH_EVENT_MOCK_H
#define MARSTECH_EVENT_MOCK_H


#include "..\IMsvEvent.h"

MSV_DISABLE_ALL_WARNINGS

#include <gmock\gmock.h>

MSV_ENABLE_WARNINGS


class MsvEvent_Mock:
	public IMsvEvent
{
public:
	MOCK_METHOD0(ResetEvent, void());
	MOCK_METHOD0(SetEvent, void(bool notifyAllThreads));
	MOCK_METHOD0(WaitForEvent, void());
	MOCK_METHOD0(WaitForEvent, void(uint32_t timeout));
	MOCK_METHOD0(WaitForEventAndReset, void());
	MOCK_METHOD0(WaitForEventAndReset, void(uint32_t timeout));
};


#endif // MARSTECH_EVENT_MOCK_H
