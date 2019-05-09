
#include "pch.h"


#include "mthreading\MsvThread.h"
#include "merror\MsvErrorCodes.h"
#include "merror\MsvException.h"


class TestMsvThreadObject:
	public MsvThread
{
public:
	TestMsvThreadObject(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spSharedConditionPredicate):
		MsvThread(spConditionVariable, spConditionVariableMutex, spSharedConditionPredicate),
		m_HandleCaughtExceptionCalls(0),
		m_OnThreadStartCalls(0),
		m_OnThreadStopCalls(0),
		m_ThreadMainCalls(0),
		m_throwException(false)
	{

	}

	TestMsvThreadObject():
		TestMsvThreadObject(std::shared_ptr<std::condition_variable>(new (std::nothrow) std::condition_variable()), std::shared_ptr<std::mutex>(new (std::nothrow) std::mutex), nullptr/*std::shared_ptr<std::uint64_t>(new (std::nothrow) std::uint64_t(0))*/)
	{

	}
	
	virtual void HandleCaughtException(const std::exception_ptr pException)
	{
		++m_HandleCaughtExceptionCalls;
	}

	virtual void OnThreadStart()
	{
		++m_OnThreadStartCalls;
		if (m_throwException)
		{
			MSV_THROW(MSV_ALLOCATION_ERROR, "Throwing exception");
		}
	}

	virtual void OnThreadStop()
	{
		++m_OnThreadStopCalls;
	}

	virtual void ThreadMain()
	{
		++m_ThreadMainCalls;
	}

	virtual void SetSharedConditionVariable(std::shared_ptr<std::condition_variable> spConditionVariable)
	{
		m_spConditionVariable = spConditionVariable;
	}

	int32_t m_HandleCaughtExceptionCalls;
	int32_t m_OnThreadStartCalls;
	int32_t m_OnThreadStopCalls;
	int32_t m_ThreadMainCalls;

	bool m_throwException;
};


class MsvThreadTests_Integration:
	public::testing::Test
{
public:
	MsvThreadTests_Integration()
	{

	}

	virtual void SetUp()
	{
		m_spThread.reset(new (std::nothrow) TestMsvThreadObject());

		EXPECT_NE(m_spThread, nullptr);
	}

	virtual void TearDown()
	{
		m_spThread.reset();
	}

	//tested class
	std::shared_ptr<TestMsvThreadObject> m_spThread;
};

TEST_F(MsvThreadTests_Integration, ItShouldNotBeRunningAfterCreate)
{
	EXPECT_FALSE(m_spThread->IsRunning());
	EXPECT_EQ(m_spThread->m_HandleCaughtExceptionCalls, 0);
	EXPECT_EQ(m_spThread->m_OnThreadStartCalls, 0);
	EXPECT_EQ(m_spThread->m_OnThreadStopCalls, 0);
	EXPECT_EQ(m_spThread->m_ThreadMainCalls, 0);
}

//this is possible dangerous test -> it deletes previous thread while it is still running
//good example what happens if you call StartThread several times
/*TEST_F(MsvThreadTests_Integration, ItShouldFailedWhenTriesToStartAlreadyStartedThread)
{
	EXPECT_FALSE(MSV_FAILED(m_spThread->StartThread(0)));
	EXPECT_EQ(m_spThread->StartThread(0), MSV_ALREADY_RUNNING_INFO);
}*/

TEST_F(MsvThreadTests_Integration, ItShouldReturnInfoWhenTriesToStopStoppedThread)
{
	EXPECT_EQ(m_spThread->StopThread(), MSV_NOT_RUNNING_INFO);
	EXPECT_FALSE(m_spThread->IsRunning());
}

TEST_F(MsvThreadTests_Integration, ItShouldReturnInfoWhenTriesToStopThreadTwice)
{
	EXPECT_FALSE(MSV_FAILED(m_spThread->StartThread(0)));
	EXPECT_FALSE(MSV_FAILED(m_spThread->StopThread()));
	EXPECT_EQ(m_spThread->StopThread(), MSV_ALREADY_REQUESTED_INFO);
}

TEST_F(MsvThreadTests_Integration, ItShouldReturnInfoWhenTriesToWaitOntoStoppedThread)
{
	EXPECT_EQ(m_spThread->WaitForThreadStop(3000000), MSV_NOT_RUNNING_INFO);
	EXPECT_FALSE(m_spThread->IsRunning());
}

TEST_F(MsvThreadTests_Integration, ItShouldReturnInfoWhenTriesToStopAndWaitForStoppedThread)
{
	EXPECT_EQ(m_spThread->StopAndWaitForThreadStop(3000000), MSV_NOT_RUNNING_INFO);
	EXPECT_FALSE(m_spThread->IsRunning());
}

TEST_F(MsvThreadTests_Integration, ItShouldReturnInfoWhenTriesAndWaitForToStopThreadTwice)
{
	EXPECT_FALSE(MSV_FAILED(m_spThread->StartThread(0)));
	EXPECT_FALSE(MSV_FAILED(m_spThread->StopAndWaitForThreadStop(3000000)));
	EXPECT_EQ(m_spThread->StopAndWaitForThreadStop(0), MSV_NOT_RUNNING_INFO);
}

TEST_F(MsvThreadTests_Integration, ItShouldRunOnlyOnceWhenNegativeTimeoutIsSet)
{
	m_spThread->StartThread(-1);
	//wait for thread stop (stop request is set in StartThread because of negative timeout)
	m_spThread->WaitForThreadStop(3000000);

	EXPECT_FALSE(m_spThread->IsRunning());
	EXPECT_EQ(m_spThread->m_HandleCaughtExceptionCalls, 0);
	EXPECT_EQ(m_spThread->m_OnThreadStartCalls, 1);
	EXPECT_EQ(m_spThread->m_OnThreadStopCalls, 1);
	EXPECT_EQ(m_spThread->m_ThreadMainCalls, 1);
}

TEST_F(MsvThreadTests_Integration, ItShouldCallHandleCaughtExceptionWhenExceptionIsCaught)
{
	m_spThread->m_throwException = true;
	m_spThread->StartThread(-1);
	//wait for thread stop (stop request is set in StartThread because of negative timeout)
	m_spThread->WaitForThreadStop(3000000);

	EXPECT_FALSE(m_spThread->IsRunning());
	EXPECT_EQ(m_spThread->m_HandleCaughtExceptionCalls, 1);
	EXPECT_EQ(m_spThread->m_OnThreadStartCalls, 1);

	//OnThreadStart throwed exception -> has not been called
	EXPECT_EQ(m_spThread->m_OnThreadStopCalls, 0);
	EXPECT_EQ(m_spThread->m_ThreadMainCalls, 0);
}

TEST_F(MsvThreadTests_Integration, ItShouldWaitForNotifyAndExecuteMaindWhenZeroTimeoutIsSet)
{
	m_spThread->StartThread(0);
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
	EXPECT_TRUE(m_spThread->IsRunning());
	m_spThread->Notify();
	std::this_thread::sleep_for(std::chrono::microseconds(1000));

	//stop and wait for thread stop
	m_spThread->StopAndWaitForThreadStop(3000000);

	EXPECT_FALSE(m_spThread->IsRunning());
	EXPECT_EQ(m_spThread->m_HandleCaughtExceptionCalls, 0);
	EXPECT_EQ(m_spThread->m_OnThreadStartCalls, 1);
	EXPECT_EQ(m_spThread->m_OnThreadStopCalls, 1);
	EXPECT_EQ(m_spThread->m_ThreadMainCalls, 2);
}

TEST_F(MsvThreadTests_Integration, ItShouldExecuteInLoopWhenPositiveTimeoutIsSet)
{
	m_spThread->StartThread(100);
	std::this_thread::sleep_for(std::chrono::microseconds(3000));

	//stop and wait for thread stop
	m_spThread->StopAndWaitForThreadStop(3000000);

	EXPECT_FALSE(m_spThread->IsRunning());
	EXPECT_EQ(m_spThread->m_HandleCaughtExceptionCalls, 0);
	EXPECT_EQ(m_spThread->m_OnThreadStartCalls, 1);
	EXPECT_EQ(m_spThread->m_OnThreadStopCalls, 1);
	EXPECT_GT(m_spThread->m_ThreadMainCalls, 1);
}

TEST_F(MsvThreadTests_Integration, ItShouldFailedWhenSharedConditionIsNull)
{
	m_spThread->SetSharedConditionVariable(nullptr);
	EXPECT_EQ(m_spThread->StartThread(-1), MSV_ALLOCATION_ERROR);
	EXPECT_FALSE(m_spThread->IsRunning());
}
