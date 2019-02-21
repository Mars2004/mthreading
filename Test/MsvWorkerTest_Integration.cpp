
#include "pch.h"


#include "mthreading\MsvWorker.h"
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

class MsvWorkerTests_Integration:
	public::testing::Test
{
public:
	MsvWorkerTests_Integration():
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
	std::function<void(void*)> m_voidContextFunction = [](void* pContext) { reinterpret_cast<MsvWorkerTests_Integration*>(pContext)->AddCall(); };
	std::shared_ptr<MsvTestTask> m_spTask;

	//counter and its lock for test functions
	std::recursive_mutex m_lock;
	int32_t m_callCount;
};

TEST_F(MsvWorkerTests_Integration, ItShouldExecuteAllRegisteredTasks)
{
	std::shared_ptr<IMsvWorker> spWorker(new (std::nothrow) MsvWorker());
	EXPECT_NE(spWorker, nullptr);

	//tasks can be added before start
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);

	EXPECT_EQ(spWorker->StartThread(0), MSV_SUCCESS);
	EXPECT_TRUE(spWorker->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);
	spWorker->AddTask(m_spTask);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spWorker->StopAndWaitForThreadStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spWorker->IsRunning());

	EXPECT_EQ(m_spTask->GetCallCount(), 20);
}

TEST_F(MsvWorkerTests_Integration, ItShouldExecuteAllRegisteredFunctions)
{
	std::shared_ptr<IMsvWorker> spWorker(new (std::nothrow) MsvWorker());
	EXPECT_NE(spWorker, nullptr);

	//tasks can be added before start
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);

	EXPECT_EQ(spWorker->StartThread(0), MSV_SUCCESS);
	EXPECT_TRUE(spWorker->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);
	spWorker->AddTask(m_voidFunction);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spWorker->StopAndWaitForThreadStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spWorker->IsRunning());

	EXPECT_EQ(GetCallCount(), 20);
}

TEST_F(MsvWorkerTests_Integration, ItShouldExecuteAllRegisteredFunctionsWithContext)
{
	std::shared_ptr<IMsvWorker> spWorker(new (std::nothrow) MsvWorker());
	EXPECT_NE(spWorker, nullptr);

	//tasks can be added before start
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);

	EXPECT_EQ(spWorker->StartThread(0), MSV_SUCCESS);
	EXPECT_TRUE(spWorker->IsRunning());

	//wait for thread start finish -> checks threads wake up
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);
	spWorker->AddTask(m_voidContextFunction, this);

	//wait for all tasks execution
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	EXPECT_EQ(spWorker->StopAndWaitForThreadStop(3000000), MSV_SUCCESS);
	EXPECT_FALSE(spWorker->IsRunning());

	EXPECT_EQ(GetCallCount(), 20);
}
