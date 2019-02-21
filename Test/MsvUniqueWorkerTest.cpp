
#include "pch.h"


#include "mthreading\MsvUniqueWorker.h"
#include "merror\MsvErrorCodes.h"

#include "mthreading\Mocks\MsvWorker_Factory_Mock.h"
#include "mthreading\Mocks\MsvTask_Mock.h"


using namespace ::testing;


class TestMsvUniqueWorkerObject:
	public MsvUniqueWorker
{
public:
	TestMsvUniqueWorkerObject(std::shared_ptr<MsvWorker_Factory> spFactory):
		MsvUniqueWorker(spFactory)
	{

	}

	void ExecuteTheadMain()
	{
		ThreadMain();
	}

	const std::shared_ptr<IMsvTask>& GetTask()
	{
		return m_spTask;
	}
};


class MsvUniqueWorkerTests:
	public::testing::Test
{
public:
	MsvUniqueWorkerTests()
	{

	}

	virtual void SetUp()
	{
		m_spWorkerFactoryMock.reset(new (std::nothrow) MsvWorker_Factory_Mock());
		m_spTask.reset(new (std::nothrow) MsvTask_Mock());

		EXPECT_NE(m_spWorkerFactoryMock, nullptr);
		EXPECT_NE(m_spTask, nullptr);

		m_spUniqueWorker.reset(new (std::nothrow) TestMsvUniqueWorkerObject(m_spWorkerFactoryMock));
		EXPECT_NE(m_spUniqueWorker, nullptr);
	}

	virtual void TearDown()
	{
		//clean all loggers before delete test log files
		m_spUniqueWorker.reset();
	}

	//mocks
	std::shared_ptr<MsvWorker_Factory_Mock> m_spWorkerFactoryMock;
	std::shared_ptr<MsvTask_Mock> m_spTask;

	//test functions
	std::function<void()> m_voidFunction = []() {};
	std::function<void(void*)> m_voidContextFunction = [](void*) {};

	//tested class
	std::shared_ptr<TestMsvUniqueWorkerObject> m_spUniqueWorker;
};

TEST_F(MsvUniqueWorkerTests, TaskShouldBeEmptyAfterCreation)
{
	EXPECT_EQ(m_spUniqueWorker->GetTask(), nullptr);
}

TEST_F(MsvUniqueWorkerTests, ItShouldHaveOneTaskAfterInsertion)
{
	EXPECT_EQ(m_spUniqueWorker->SetTask(m_spTask), MSV_SUCCESS);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
}

TEST_F(MsvUniqueWorkerTests, ItShouldReturnInfoAfterSecondInsertion)
{
	EXPECT_EQ(m_spUniqueWorker->SetTask(m_spTask), MSV_SUCCESS);
	EXPECT_EQ(m_spUniqueWorker->SetTask(m_spTask), MSV_ALREADY_SET_INFO);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
}

TEST_F(MsvUniqueWorkerTests, ItShouldHaveOneTaskAfterInsertionVoidFunction)
{
	std::function<void()> voidFunction;
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(DoAll(SaveArg<0>(&voidFunction), Return(m_spTask)));

	EXPECT_EQ(m_spUniqueWorker->SetTask(m_voidFunction), MSV_SUCCESS);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
	EXPECT_EQ(voidFunction.target<std::function<void()>>(), m_voidFunction.target<std::function<void()>>());
}

TEST_F(MsvUniqueWorkerTests, SetTaskShouldFailedWhenNullptrIsReturnedForVoidFunction)
{
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void()>&>(_)))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spUniqueWorker->SetTask(m_voidFunction), MSV_ALLOCATION_ERROR);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), nullptr);
}

TEST_F(MsvUniqueWorkerTests, ItShouldHaveOneTaskAfterInsertionVoidContextFunction)
{
	std::function<void(void*)> voidContextFunction;
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(DoAll(SaveArg<0>(&voidContextFunction), Return(m_spTask)));

	EXPECT_EQ(m_spUniqueWorker->SetTask(m_voidContextFunction, this), MSV_SUCCESS);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
	EXPECT_EQ(voidContextFunction.target<std::function<void(void*)>>(), m_voidContextFunction.target<std::function<void(void*)>>());
}

TEST_F(MsvUniqueWorkerTests, SetTaskShouldFailedWhenNullptrIsReturnedForVoidContextFunction)
{
	EXPECT_CALL(*m_spWorkerFactoryMock, GetIMsvTask(Matcher<std::function<void(void*)>&>(_), this))
		.WillOnce(Return(nullptr));

	EXPECT_EQ(m_spUniqueWorker->SetTask(m_voidContextFunction, this), MSV_ALLOCATION_ERROR);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), nullptr);
}

TEST_F(MsvUniqueWorkerTests, ItShouldCallExecuteMethodOfInsertedTask)
{
	EXPECT_EQ(m_spUniqueWorker->SetTask(m_spTask), MSV_SUCCESS);

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
	
	//three work items -> should be called three times
	EXPECT_CALL(*m_spTask, Execute());

	//execute
	m_spUniqueWorker->ExecuteTheadMain();

	EXPECT_EQ(m_spUniqueWorker->GetTask(), m_spTask);
}
