/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Pool Implementation
* @details		Contains implementation of @ref MsvThread.
* @author		Martin Svoboda
* @date			26.11.2018
* @copyright	GNU General Public License (GPLv3).
******************************************************************************************************/


/*
This file is part of MarsTech Threading.

MarsTech Dependency Injection is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MarsTech Promise Like Syntax is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/


#include "MsvThreadPool.h"
#include "MsvThreadPool_Factory.h"

#include "merror/MsvErrorCodes.h"


/********************************************************************************************************************************
*                                              Constructors and destructors
********************************************************************************************************************************/


MsvThreadPool::MsvThreadPool(std::shared_ptr<MsvThreadPool_Factory> spFactory):
	m_isRunning(false),
	m_spSharedCondition(new (std::nothrow) std::condition_variable),
	m_spSharedConditionMutex(new (std::nothrow) std::mutex),
	m_spSharedConditionPredicate(new (std::nothrow) uint64_t(0)),
	m_stopRequested(false),
	m_spFactory(spFactory ? spFactory : MsvThreadPool_Factory::Get())
{
}

MsvThreadPool::~MsvThreadPool()
{
	//it is called with default timeout (30s)
	StopAndWaitForThreadPoolStop();
}


/********************************************************************************************************************************
*															IMsvThread public methods
********************************************************************************************************************************/


void MsvThreadPool::AddTask(std::shared_ptr<IMsvTask> task)
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	m_taskQueue.push(task);

	std::unique_lock<std::mutex> conditionLock(*m_spSharedConditionMutex);
	//++ because of m_spSharedCondition->notify_one() -> one thread is woken up -> one thread will check task queue
	(*m_spSharedConditionPredicate)++;
	conditionLock.unlock();

	m_spSharedCondition->notify_one();
}

MsvErrorCode MsvThreadPool::AddTask(std::function<void()>& task)
{
	//create task
	std::shared_ptr<IMsvTask> spTask;
	if (m_spFactory)
	{
		spTask = m_spFactory->GetIMsvTask(task);
	}

	//add task to queue
	if (spTask)
	{
		AddTask(spTask);
	}
	else
	{
		return MSV_ALLOCATION_ERROR;
	}

	return MSV_SUCCESS;
}

MsvErrorCode MsvThreadPool::AddTask(std::function<void(void*)>& task, void* pContext)
{
	//create task
	std::shared_ptr<IMsvTask> spTask;
	if (m_spFactory)
	{
		spTask = m_spFactory->GetIMsvTask(task, pContext);
	}

	//add task to queue
	if (spTask)
	{
		AddTask(spTask);
	}
	else
	{
		return MSV_ALLOCATION_ERROR;
	}

	return MSV_SUCCESS;
}

bool MsvThreadPool::IsRunning() const
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	return m_isRunning;
}

MsvErrorCode MsvThreadPool::StartThreadPool(uint16_t threadCount)
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);

	if (IsRunning())
	{
		return MSV_ALREADY_RUNNING_INFO;
	}

	if (!m_spSharedCondition || !m_spSharedConditionMutex || !m_spSharedConditionPredicate)
	{
		return MSV_ALLOCATION_ERROR;
	}

	//create workers
	for (int i = 0; i < threadCount; ++i)
	{
		std::shared_ptr<IMsvUniqueWorker> spWorkerThread(m_spFactory->GetIMsvUniqueWorker(m_spSharedCondition, m_spSharedConditionMutex, m_spSharedConditionPredicate));
		if (!spWorkerThread)
		{
			m_workers.clear();
			return MSV_ALLOCATION_ERROR;
		}

		m_workers.push_back(spWorkerThread);
	}

	MsvErrorCode errorCode = MSV_SUCCESS;

	//create callback for workers
	std::function<void()> callback = std::bind(&MsvThreadPool::ExecuteTask, this);

	//start workers
	std::vector<std::shared_ptr<IMsvUniqueWorker>>::iterator endIt = m_workers.end();
	for (std::vector<std::shared_ptr<IMsvUniqueWorker>>::iterator it = m_workers.begin(); it != endIt; ++it)
	{
		//execute and wait for notify thread mode
		if (MSV_FAILED(errorCode = (*it)->SetTask(callback)) || MSV_FAILED(errorCode = (*it)->StartThread()))
		{
			//stop all threads and return errorcode
			m_workers.clear();
			return errorCode;
		}
	}

	m_isRunning = true;

	return errorCode;
}

MsvErrorCode MsvThreadPool::StopThreadPool()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);

	if (!IsRunning())
	{
		return MSV_NOT_RUNNING_INFO;
	}
	else if (m_stopRequested)
	{
		return MSV_ALREADY_REQUESTED_INFO;
	}

	m_stopRequested = true;

	MsvErrorCode result = MSV_SUCCESS;
	//stop workers
	std::vector<std::shared_ptr<IMsvUniqueWorker>>::const_iterator endIt = m_workers.end();
	for (std::vector<std::shared_ptr<IMsvUniqueWorker>>::const_iterator it = m_workers.begin(); it != endIt; ++it)
	{
		MsvErrorCode errorCode = (*it)->StopThread();
		if (MSV_FAILED(errorCode))
		{
			//only last error code of failed thread stop request
			result = errorCode;
		}
	}

	return result;
}

MsvErrorCode MsvThreadPool::StopAndWaitForThreadPoolStop(int32_t timeout)
{
	//it is OK to do like this (StopThreadPool checks if thread pool is running)
	MsvErrorCode errorCode = StopThreadPool();
	MSV_RETURN_FAILED(WaitForThreadPoolStop(timeout));

	return errorCode;
}

MsvErrorCode MsvThreadPool::WaitForThreadPoolStop(int32_t timeout)
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);

	if (!IsRunning())
	{
		return MSV_NOT_RUNNING_INFO;
	}
	else if (!m_stopRequested)
	{
		return MSV_NOT_REQUESTED_ERROR;
	}
	
	MsvErrorCode result = MSV_SUCCESS;

	//wait for workers to stop
	std::vector<std::shared_ptr<IMsvUniqueWorker>>::iterator endIt = m_workers.end();
	for (std::vector<std::shared_ptr<IMsvUniqueWorker>>::iterator it = m_workers.begin(); it != endIt; ++it)
	{
		MsvErrorCode errorCode = (*it)->WaitForThreadStop(timeout);
		if (MSV_FAILED(errorCode))
		{
			//only last error code of failed wait
			result = errorCode;
		}
	}

	m_isRunning = false;

	return result;
}


/********************************************************************************************************************************
*															MsvThread protected methods
********************************************************************************************************************************/


void MsvThreadPool::ExecuteTask()
{
	std::shared_ptr<IMsvTask> spTask;

	//it might block thread pool stopping (because of many tasks or long time running tasks in the queue)

	//execute tasks until exists any task in the queue
	while ((spTask = GetTask()) != nullptr)
	{
		spTask->Execute();
	}
}

std::shared_ptr<IMsvTask> MsvThreadPool::GetTask()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	if (m_taskQueue.empty())
	{
		return nullptr;
	}

	std::shared_ptr<IMsvTask> spTmpTask = m_taskQueue.front();
	m_taskQueue.pop();

	return spTmpTask;
}


/** @} */	//End of group MTHREADING.
