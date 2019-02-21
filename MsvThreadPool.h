/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Pool Implementation
* @details		Contains implementation @ref MsvThreadPool of @ref IMsvThreadPool interface.
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


#ifndef MARSTECH_THREADPOOL_H
#define MARSTECH_THREADPOOL_H


#include "IMsvThreadPool.h"

#include "IMsvUniqueWorker.h"

MSV_DISABLE_ALL_WARNINGS

#include <mutex>
#include <vector>
#include <queue>

MSV_ENABLE_WARNINGS


//forward declaration of MarsTech Thread Pool Dependency Injection Factory
class MsvThreadPool_Factory;


/**************************************************************************************************//**
* @brief		MarsTech Thread Pool Implementation.
* @details	Implementation of thread pool for easy threading.
* @see		IMsvThreadPool
******************************************************************************************************/
class MsvThreadPool:
	public IMsvThreadPool
{
public:
	/**************************************************************************************************//**
	* @brief		Constructor.
	* @param		spFactory		Shared pointer to dependency injection factory.
	* @see		MsvWorker_Factory
	******************************************************************************************************/
	MsvThreadPool(std::shared_ptr<MsvThreadPool_Factory> spFactory = nullptr);

	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~MsvThreadPool();

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::AddTask(std::shared_ptr<IMsvTask> spTask)
	******************************************************************************************************/
	virtual void AddTask(std::shared_ptr<IMsvTask> spTask) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::AddTask(std::function<void()>& task)
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void()>& task) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::AddTask(std::function<void(void*)>& task, void* pContext)
	******************************************************************************************************/
	virtual MsvErrorCode AddTask(std::function<void(void*)>& task, void* pContext) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::IsRunning()
	******************************************************************************************************/
	bool IsRunning() const override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::StartThreadPool(uint16_t threadCount)
	******************************************************************************************************/
	virtual MsvErrorCode StartThreadPool(uint16_t threadCount) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::StopThreadPool()
	******************************************************************************************************/
	virtual MsvErrorCode StopThreadPool() override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::StopAndWaitForThreadPoolStop(int32_t timeout)
	******************************************************************************************************/
	virtual MsvErrorCode StopAndWaitForThreadPoolStop(int32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThreadPool::WaitForThreadPoolStop(int32_t timeout)
	******************************************************************************************************/
	virtual MsvErrorCode WaitForThreadPoolStop(int32_t timeout) override;

protected:
	/**************************************************************************************************//**
	* @brief			Task execution function.
	* @details		This is callback inserted to each worker (instance of @ref IMsvUniqueWorker). It calls
	*					execute function of tasks.
	* @see			GetTask
	******************************************************************************************************/
	void ExecuteTask();

	/**************************************************************************************************//**
	* @brief			Get task to execute.
	* @details		Returns current task in the queue @ref m_taskQueue which will be executed. The task
	*					is removed from the queue.
	* @returns		std::shared_ptr<IMsvTask>
	* @see			m_taskQueue
	******************************************************************************************************/
	virtual std::shared_ptr<IMsvTask> GetTask();

protected:
	/**************************************************************************************************//**
	* @brief		Flag if thread pool is running (true) or not (false).
	* @see		IsRunning
	******************************************************************************************************/
	bool m_isRunning;

	/**************************************************************************************************//**
	* @brief		Thread pool mutex.
	* @details	Locks this object for thread safety access.
	******************************************************************************************************/
	mutable std::recursive_mutex m_lock;

	/**************************************************************************************************//**
	* @brief		Dependency injection factory.
	* @details	Contains get method for all injected objects.
	* @see		MsvThreadPool_Factory
	******************************************************************************************************/
	std::shared_ptr<MsvThreadPool_Factory> m_spFactory;

	/**************************************************************************************************//**
	* @brief		Shared condition variable.
	* @details	It is used for worker thread synchronization (wake ups thread if new task is inserted).
	* @see		AddTask
	******************************************************************************************************/
	std::shared_ptr<std::condition_variable> m_spSharedCondition;

	std::shared_ptr<std::mutex> m_spSharedConditionMutex;

	std::shared_ptr<uint64_t> m_spSharedConditionPredicate;

	/**************************************************************************************************//**
	* @brief		Flag if thread pool stop is requested (true) or not (false).
	* @details	When this flag is set (true), worker threads will be stopped.
	* @see		StopThreadPool
	******************************************************************************************************/
	bool m_stopRequested;

	/**************************************************************************************************//**
	* @brief		Task queue.
	* @details	Contains all inserted tasks for execution.
	* @see		AddTask
	******************************************************************************************************/
	std::queue<std::shared_ptr<IMsvTask>> m_taskQueue;

	/**************************************************************************************************//**
	* @brief		Worker threads.
	* @details	Contains all worker threads.
	******************************************************************************************************/
	std::vector<std::shared_ptr<IMsvUniqueWorker>> m_workers;
};


#endif // !MARSTECH_ITHREADPOOL_H

/** @} */	//End of group MTHREADING.
