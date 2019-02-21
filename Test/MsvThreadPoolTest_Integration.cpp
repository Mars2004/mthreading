
#include "pch.h"


#include "mthreading\MsvThreadPool.h"
#include "merror\MsvErrorCodes.h"
#include "merror\MsvException.h"

#include <thread>


using namespace ::testing;


class MsvTestTask:
	public IMsvTask
{
public:
	MsvTestTask():
		m_callCount(0)
	{

	}

	virtual void Execute()
	{
		std::lock_guard<std::recursive_mutex> lock(m_lock);
		++m_callCount;
	}

	int32_t GetCallCount()
	{
		std::lock_guard<std::recursive_mutex> lock(m_lock);
		return m_callCount;
	}

protected:
	std::recursive_mutex m_lock;
	int32_t m_callCount;
};

class MsvThreadPoolTests_Integration:
	public::testing::Test
{
public:
	MsvThreadPoolTests_Integration():
		m_callCount(0)
	{

	}

	virtual void SetUp()
	{
		m_spTask.reset(new (std::nothrow) MsvTestTask());
		EXPECT_NE(m_spTask, nullptr);
	}

	virtual void TearDown()
	{
		m_spTask.reset();
	}

	void AddCall()
	{
		std::lock_guard<std::recursive_mutex> lock(m_lock);
		++m_callCount;
	}

	int32_t GetCallCount()
	{
		std::lock_guard<std::recursive_mutex> lock(m_lock);
		return m_callCount;
	}

	//test functions anc classes
	std::function<void()> m_voidFunction = [this]() { this->AddCall(); };
	std::function<void(void*)> m_voidContextFunction = [](void* pContext) { reinterpret_cast<MsvThreadPoolTests_Integration*>(pContext)->AddCall(); };
	std::shared_ptr<MsvTestTask> m_spTask;

	//counter and its lock for test functions
	std::recursive_mutex m_lock;
	int32_t m_callCount;
};

TEST_F(MsvThreadPoolTests_Integration, ItShouldExecuteAllRegisteredTasks)
{
	std::shared_ptr<IMsvThreadPool> spThreadPool(new (std::nothrow) MsvThreadPool());
	EXPECT_NE(spThreadPool, nullptr);

	//tasks can be added before start
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);

	EXPECT_EQ(spThreadPool->StartThreadPool(3), MSV_SUCCESS);
	EXPECT_TRUE(spThreadPool->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);
	spThreadPool->AddTask(m_spTask);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spThreadPool->StopAndWaitForThreadPoolStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spThreadPool->IsRunning());

	EXPECT_EQ(m_spTask->GetCallCount(), 20);
}

TEST_F(MsvThreadPoolTests_Integration, ItShouldExecuteAllRegisteredFunctions)
{
	std::shared_ptr<IMsvThreadPool> spThreadPool(new (std::nothrow) MsvThreadPool());
	EXPECT_NE(spThreadPool, nullptr);

	//tasks can be added before start
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);

	EXPECT_EQ(spThreadPool->StartThreadPool(3), MSV_SUCCESS);
	EXPECT_TRUE(spThreadPool->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);
	spThreadPool->AddTask(m_voidFunction);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spThreadPool->StopAndWaitForThreadPoolStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spThreadPool->IsRunning());

	EXPECT_EQ(GetCallCount(), 20);
}

TEST_F(MsvThreadPoolTests_Integration, ItShouldExecuteAllRegisteredFunctionsWithContext)
{
	std::shared_ptr<IMsvThreadPool> spThreadPool(new (std::nothrow) MsvThreadPool());
	EXPECT_NE(spThreadPool, nullptr);

	//tasks can be added before start
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);

	EXPECT_EQ(spThreadPool->StartThreadPool(3), MSV_SUCCESS);
	EXPECT_TRUE(spThreadPool->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);
	spThreadPool->AddTask(m_voidContextFunction, this);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spThreadPool->StopAndWaitForThreadPoolStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spThreadPool->IsRunning());

	EXPECT_EQ(GetCallCount(), 20);
}
