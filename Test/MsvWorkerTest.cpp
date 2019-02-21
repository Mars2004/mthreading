
#include "pch.h"


#include "mthreading\MsvWorker.h"
#include "merror\MsvErrorCodes.h"

#include "mthreading\Mocks\MsvWorker_Factory_Mock.h"
#include "mthreading\Mocks\MsvTask_Mock.h"


using namespace ::testing;


class TestMsvWorkerObject:
	public MsvWorker
{
public:
	TestMsvWorkerObject(std::shared_ptr<MsvWorker_Factory> spFactory):
		MsvWorker(spFactory)
	{

	}

	void ExecuteTheadMain()
	{
		ThreadMain();
	}

	const std::queue<std::shared_ptr<IMsvTask>>& GetTasks()
	{
		return m_tasks;
	}
};


class MsvWorkerTests:
	public::testing::Test
{
public:
	MsvWorkerTests()
	{

	}

	virtual void SetUp()
	{
		m_spWorkerFactoryMock.reset(new (std::nothrow) MsvWorker_Factory_Mock());
		m_spTask.reset(new (std::nothrow) MsvTask_Mock());

		EXPECT_NE(m_spWorkerFactoryMock, nullptr);
		EXPECT_NE(m_spTask, nullptr);

		m_spWorker.reset(new (std::nothrow) TestMsvWorkerObject(m_spWorkerFactoryMock));
		EXPECT_NE(m_spWorker, nullptr);
	}

	virtual void TearDown()
	{
		//clean all loggers before delete test log files
		m_spWorker.reset();
	}

	//mocks
	std::shared_ptr<MsvWorker_Factory_Mock> m_spWorkerFactoryMock;
	std::shared_ptr<MsvTask_Mock> m_spTask;

	//test functions
	std::function<void()> m_voidFunction = []() {};
	std::function<void(void*)> m_voidContextFunction = [](void*) {};

	//tested class
	std::shared_ptr<TestMsvWorkerObject> m_spWorker;
};

TEST_F(MsvWorkerTests, WorksShouldBeEmptyAfterCreation)
{
	EXPECT_EQ(m_spWorker->GetTasks().size(), 0);
}

TEST_F(MsvWorkerTests, ItShouldHaveOneWorkAfterInsertion)
{
	m_spWorker->AddTask(m_spTask);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 1);
	EXPECT_EQ(m_spWorker->GetTasks().front(), m_spTask);
}

TEST_F(MsvWorkerTests, ItShouldHaveOneWorkAfterInsertionVoidFunction)
{
	std::function<void()> voidFunction;
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(DoAll(SaveArg<0>(&voidFunction), Return(m_spTask)));

	EXPECT_EQ(m_spWorker->AddTask(m_voidFunction), MSV_SUCCESS);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 1);
	EXPECT_EQ(m_spWorker->GetTasks().front(), m_spTask);
	EXPECT_EQ(voidFunction.target<std::function<void()>>(), m_voidFunction.target<std::function<void()>>());
}

TEST_F(MsvWorkerTests, AddTaskShouldFailedWhenNullptrIsReturnedForVoidFunction)
{
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spWorker->AddTask(m_voidFunction), MSV_ALLOCATION_ERROR);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 0);
}

TEST_F(MsvWorkerTests, ItShouldHaveOneWorkAfterInsertionVoidContextFunction)
{
	std::function<void(void*)> voidContextFunction;
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(DoAll(SaveArg<0>(&voidContextFunction), Return(m_spTask)));

	EXPECT_EQ(m_spWorker->AddTask(m_voidContextFunction, this), MSV_SUCCESS);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 1);
	EXPECT_EQ(m_spWorker->GetTasks().front(), m_spTask);
	EXPECT_EQ(voidContextFunction.target<std::function<void(void*)>>(), m_voidContextFunction.target<std::function<void(void*)>>());
}

TEST_F(MsvWorkerTests, AddTaskShouldFailedWhenNullptrIsReturnedForVoidContextFunction)
{
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spWorker->AddTask(m_voidContextFunction, this), MSV_ALLOCATION_ERROR);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 0);
}

TEST_F(MsvWorkerTests, ItShouldCallExecuteMethodOfAllInsertedWorks)
{
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(m_spTask));

	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(Return(m_spTask));

	m_spWorker->AddTask(m_spTask);
	EXPECT_EQ(m_spWorker->AddTask(m_voidFunction), MSV_SUCCESS);
	EXPECT_EQ(m_spWorker->AddTask(m_voidContextFunction, this), MSV_SUCCESS);

	EXPECT_EQ(m_spWorker->GetTasks().size(), 3);
	
	//three work items -> should be called three times
	EXPECT_CALL(*m_spTask, Execute())
		.Times(3);

	//execute
	m_spWorker->ExecuteTheadMain();

	EXPECT_EQ(m_spWorker->GetTasks().size(), 0);
}
