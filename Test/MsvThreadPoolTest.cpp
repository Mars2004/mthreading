
#include "pch.h"


#include "mthreading\MsvThreadPool.h"
#include "merror\MsvErrorCodes.h"
#include "merror\MsvException.h"

#include "mthreading\Mocks\MsvThreadPool_Factory_Mock.h"
#include "mthreading\Mocks\MsvUniqueWorker_Mock.h"
#include "mthreading\Mocks\MsvTask_Mock.h"


using namespace ::testing;


class TestMsvThreadPoolObject:
	public MsvThreadPool
{
public:
	TestMsvThreadPoolObject(std::shared_ptr<MsvThreadPool_Factory> spFactory = nullptr):
		MsvThreadPool(spFactory)
	{

	}

	/*const */std::queue<std::shared_ptr<IMsvTask>>& GetTasks()
	{
		return m_taskQueue;
	}

	const std::vector<std::shared_ptr<IMsvUniqueWorker>>& GetWorkers()
	{
		return m_workers;
	}

	std::shared_ptr<std::condition_variable>& GetSharedCondition()
	{
		return m_spSharedCondition;
	}

	std::shared_ptr<std::mutex>& GetSharedMutex()
	{
		return m_spSharedConditionMutex;
	}

	std::shared_ptr<std::uint64_t>& GetSharedPredicate()
	{
		return m_spSharedConditionPredicate;
	}
};

class MsvThreadPoolTests:
	public::testing::Test
{
public:
	MsvThreadPoolTests()
	{

	}

	virtual void SetUp()
	{
		m_spThreadPoolFactoryMock.reset(new (std::nothrow) MsvThreadPool_Factory_Mock());
		m_spTask.reset(new (std::nothrow) MsvTask_Mock());
		m_spUniqueWorker.reset(new (std::nothrow) MsvUniqueWorker_Mock());

		EXPECT_NE(m_spThreadPoolFactoryMock, nullptr);
		EXPECT_NE(m_spTask, nullptr);
		EXPECT_NE(m_spUniqueWorker, nullptr);

		m_spThreadPool.reset(new (std::nothrow) TestMsvThreadPoolObject(m_spThreadPoolFactoryMock));

		EXPECT_NE(m_spThreadPool, nullptr);
	}

	virtual void TearDown()
	{
		m_spThreadPool.reset();
	}

	//mocks
	std::shared_ptr<MsvThreadPool_Factory_Mock> m_spThreadPoolFactoryMock;
	std::shared_ptr<MsvTask_Mock> m_spTask;
	std::shared_ptr<MsvUniqueWorker_Mock> m_spUniqueWorker;

	//test functions
	std::function<void()> m_voidFunction = []() {};
	std::function<void(void*)> m_voidContextFunction = [](void*) {};

	//tested class
	std::shared_ptr<TestMsvThreadPoolObject> m_spThreadPool;
};

TEST_F(MsvThreadPoolTests, ItShouldNotBeRunningAfterCreate)
{
	EXPECT_FALSE(m_spThreadPool->IsRunning());
	EXPECT_EQ(m_spThreadPool->GetTasks().size(), 0);
	EXPECT_EQ(m_spThreadPool->GetWorkers().size(), 0);
}

TEST_F(MsvThreadPoolTests, ItShouldBeAbleToAddTaskIfThreadPoolIsNotRunning)
{
	m_spThreadPool->AddTask(m_spTask);

	std::function<void()> voidFunction;
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(DoAll(SaveArg<0>(&voidFunction), Return(m_spTask)));

	EXPECT_EQ(m_spThreadPool->AddTask(m_voidFunction), MSV_SUCCESS);

	std::function<void(void*)> voidContextFunction;
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(DoAll(SaveArg<0>(&voidContextFunction), Return(m_spTask)));

	EXPECT_EQ(m_spThreadPool->AddTask(m_voidContextFunction, this), MSV_SUCCESS);

	EXPECT_FALSE(m_spThreadPool->IsRunning());
	EXPECT_EQ(m_spThreadPool->GetWorkers().size(), 0);

	EXPECT_EQ(m_spThreadPool->GetTasks().size(), 3);
	EXPECT_EQ(m_spThreadPool->GetTasks().front(), m_spTask);
	m_spThreadPool->GetTasks().pop();
	EXPECT_EQ(m_spThreadPool->GetTasks().front(), m_spTask);
	m_spThreadPool->GetTasks().pop();
	EXPECT_EQ(m_spThreadPool->GetTasks().front(), m_spTask);
	m_spThreadPool->GetTasks().pop();
	EXPECT_EQ(m_spThreadPool->GetTasks().size(), 0);
}

TEST_F(MsvThreadPoolTests, AddTaskShouldFailedWhenNullptrIsReturned)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spThreadPool->AddTask(m_voidFunction), MSV_ALLOCATION_ERROR);

	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spThreadPool->AddTask(m_voidContextFunction, this), MSV_ALLOCATION_ERROR);

	EXPECT_EQ(m_spThreadPool->GetTasks().size(), 0);
}

//also tests StartThreadPool success
TEST_F(MsvThreadPoolTests, StartThreadPoolShouldReturnInfoWhenAlreadyRunning)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);
	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_ALREADY_RUNNING_INFO);
	EXPECT_TRUE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StartThreadPoolShouldFailedWhenSharedConditionVariableIsNull)
{
	m_spThreadPool->GetSharedCondition().reset();

	EXPECT_EQ(static_cast<std::shared_ptr<IMsvThreadPool>>(m_spThreadPool)->StartThreadPool(), MSV_ALLOCATION_ERROR);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StartThreadPoolShouldFailedWhenGetWorkersFailed)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(static_cast<std::shared_ptr<IMsvThreadPool>>(m_spThreadPool)->StartThreadPool(), MSV_ALLOCATION_ERROR);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StartThreadPoolShouldFailedWhenSetTaskFailed)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_NOT_INITIALIZED_ERROR));

	EXPECT_EQ(static_cast<std::shared_ptr<IMsvThreadPool>>(m_spThreadPool)->StartThreadPool(1), MSV_NOT_INITIALIZED_ERROR);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StartThreadPoolShouldFailedWhenStartWorkersFailed)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_NOT_INITIALIZED_ERROR));

	EXPECT_EQ(static_cast<std::shared_ptr<IMsvThreadPool>>(m_spThreadPool)->StartThreadPool(1), MSV_NOT_INITIALIZED_ERROR);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StopThreadPoolShouldReturnInfoWhenNotRunning)
{
	EXPECT_FALSE(m_spThreadPool->IsRunning());

	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_NOT_RUNNING_INFO);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

//also tests StopThreadPool success
TEST_F(MsvThreadPoolTests, StopThreadPoolShouldReturnInfoWhenStopAlreadyRequested)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_CALL(*m_spUniqueWorker, StopThread())
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_SUCCESS);
	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_ALREADY_REQUESTED_INFO);

	//still running after stop request
	EXPECT_TRUE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StopThreadPoolShouldFailedWhenStopThreadsFailed)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_CALL(*m_spUniqueWorker, StopThread())
		.WillOnce(Return(MSV_NOT_INITIALIZED_ERROR));

	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_NOT_INITIALIZED_ERROR);

	//still running after stop request
	EXPECT_TRUE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, WaitForThreadPoolStopShouldReturnInfoWhenNotRunning)
{
	EXPECT_FALSE(m_spThreadPool->IsRunning());

	EXPECT_EQ(m_spThreadPool->WaitForThreadPoolStop(30000), MSV_NOT_RUNNING_INFO);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, WaitForThreadPoolStopShouldFailedWhenStopWasNotRequested)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_EQ(m_spThreadPool->WaitForThreadPoolStop(30000), MSV_NOT_REQUESTED_ERROR);
	EXPECT_TRUE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, WaitForThreadPoolStopShouldFailedWhenStopThreadsFailed)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StopThread())
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);
	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_CALL(*m_spUniqueWorker, WaitForThreadStop(30000))
		.WillOnce(Return(MSV_NOT_INITIALIZED_ERROR));

	EXPECT_EQ(m_spThreadPool->WaitForThreadPoolStop(30000), MSV_NOT_INITIALIZED_ERROR);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, WaitForThreadPoolStopShouldSucceeded)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StopThread())
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);
	EXPECT_EQ(m_spThreadPool->StopThreadPool(), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_CALL(*m_spUniqueWorker, WaitForThreadStop(30000))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->WaitForThreadPoolStop(30000), MSV_SUCCESS);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}

TEST_F(MsvThreadPoolTests, StopAndWaitForThreadPoolStopShouldSucceeded)
{
	EXPECT_CALL(*m_spThreadPoolFactoryMock, GetIMsvUniqueWorker(m_spThreadPool->GetSharedCondition(), m_spThreadPool->GetSharedMutex(), m_spThreadPool->GetSharedPredicate()))
		.WillOnce(Return(m_spUniqueWorker));

	EXPECT_CALL(*m_spUniqueWorker, SetTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StartThread(0))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_CALL(*m_spUniqueWorker, StopThread())
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StartThreadPool(1), MSV_SUCCESS);

	EXPECT_TRUE(m_spThreadPool->IsRunning());

	EXPECT_CALL(*m_spUniqueWorker, WaitForThreadStop(30000))
		.WillOnce(Return(MSV_SUCCESS));

	EXPECT_EQ(m_spThreadPool->StopAndWaitForThreadPoolStop(30000), MSV_SUCCESS);
	EXPECT_FALSE(m_spThreadPool->IsRunning());
}
