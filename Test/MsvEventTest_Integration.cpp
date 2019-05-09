
#include "pch.h"


#include "mthreading\MsvEvent.h"
#include "merror\MsvErrorCodes.h"
#include "merror\MsvException.h"


class MsvEventTests_Integration:
	public::testing::Test
{
public:
	MsvEventTests_Integration():
		m_timeouted(0)
	{

	}

	virtual void SetUp()
	{
		m_spEvent.reset(new (std::nothrow) MsvEvent());

		EXPECT_NE(m_spEvent, nullptr);
	}

	virtual void TearDown()
	{
		m_spEvent.reset();
	}

	void Timeouted()
	{
		std::lock_guard<std::mutex> lock(m_lock);
		++m_timeouted;
	}

	std::mutex m_lock;
	int m_timeouted;

	//tested class
	std::shared_ptr<IMsvEvent> m_spEvent;
};


TEST_F(MsvEventTests_Integration, ItShouldWaitForEvent)
{
	std::thread testThread([this] () { EXPECT_EQ(m_spEvent->WaitForEvent(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWakeUpBeforeTimeout)
{
	std::thread testThread([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(30000000), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldTimeouted)
{
	std::thread testThread([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(100), MSV_EXPIRED_INFO); });

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWakeUpAllThreadsBeforeTimeout)
{
	std::thread testThread1([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(30000000), MSV_SUCCESS); });
	std::thread testThread2([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(30000000), MSV_SUCCESS); });
	std::thread testThread3([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(30000000), MSV_SUCCESS); });
	std::thread testThread4([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(30000000), MSV_SUCCESS); });

	m_spEvent->SetEvent(true);

	testThread1.join();
	testThread2.join();
	testThread3.join();
	testThread4.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWakeUpOnlyOneThreadBeforeTimeout)
{
	std::thread testThread1([this]() { if (m_spEvent->WaitForEvent(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread2([this]() { if (m_spEvent->WaitForEvent(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread3([this]() { if (m_spEvent->WaitForEvent(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread4([this]() { if (m_spEvent->WaitForEvent(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });

	m_spEvent->SetEvent();

	testThread1.join();
	testThread2.join();
	testThread3.join();
	testThread4.join();

	EXPECT_EQ(m_timeouted, 3);
}

TEST_F(MsvEventTests_Integration, ItShouldSuccededWhenEventIsNotReseted)
{
	std::thread testThread1([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread1.join();

	std::thread testThread2([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(), MSV_SUCCESS); });
	testThread2.join();
}

TEST_F(MsvEventTests_Integration, ItShouldSuccededWhenEventIsNotReseted_Timeout)
{
	std::thread testThread1([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread1.join();

	std::thread testThread2([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(100), MSV_SUCCESS); });
	testThread2.join();
}

TEST_F(MsvEventTests_Integration, ItShouldExpiredWhenEventIsReseted)
{
	std::thread testThread1([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread1.join();
	m_spEvent->ResetEvent();

	std::thread testThread2([this]() { EXPECT_EQ(m_spEvent->WaitForEvent(100), MSV_EXPIRED_INFO); });
	testThread2.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWaitForEvent_AutoReset)
{
	std::thread testThread([this]() { EXPECT_EQ(m_spEvent->WaitForEventAndReset(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWakeUpBeforeTimeout_AutoReset)
{
	std::thread testThread([this]() { EXPECT_EQ(m_spEvent->WaitForEventAndReset(30000000), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldTimeouted_AutoReset)
{
	std::thread testThread([this]() { EXPECT_EQ(m_spEvent->WaitForEventAndReset(100), MSV_EXPIRED_INFO); });

	testThread.join();
}

TEST_F(MsvEventTests_Integration, ItShouldWakeUpOnlyOneThreadBeforeTimeout_AutoReset)
{
	std::thread testThread1([this]() { if (m_spEvent->WaitForEventAndReset(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread2([this]() { if (m_spEvent->WaitForEventAndReset(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread3([this]() { if (m_spEvent->WaitForEventAndReset(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });
	std::thread testThread4([this]() { if (m_spEvent->WaitForEventAndReset(1000000) == MSV_EXPIRED_INFO) { Timeouted(); } m_spEvent->ResetEvent(); });

	m_spEvent->SetEvent();

	testThread1.join();
	testThread2.join();
	testThread3.join();
	testThread4.join();

	EXPECT_EQ(m_timeouted, 3);
}

TEST_F(MsvEventTests_Integration, ItShouldNotExpiredWhenEventIsNotReseted_AutoReset)
{
	std::thread testThread1([this]() { EXPECT_EQ(m_spEvent->WaitForEventAndReset(), MSV_SUCCESS); });

	m_spEvent->SetEvent();

	testThread1.join();

	std::thread testThread2([this]() { EXPECT_EQ(m_spEvent->WaitForEventAndReset(100), MSV_EXPIRED_INFO); });
	testThread2.join();
}