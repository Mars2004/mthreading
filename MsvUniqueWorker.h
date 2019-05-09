/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Unique Worker Implementation
* @details		Contains implementation @ref MsvUniqueWorker of @ref IMsvUniqueWorker interface.
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


#ifndef MARSTECH_UNIQUEWORKER_H
#define MARSTECH_UNIQUEWORKER_H


#include "IMsvUniqueWorker.h"
#include "MsvThread.h"

#include "IMsvTask.h"


//forward declaration of MarsTech Worker Dependency Injection Factory
class MsvWorker_Factory;


/**************************************************************************************************//**
* @brief		MarsTech Unique Worker Implementation.
* @details	Implementation of worker which executes one unique job/task (function or @ref IMsvTask).
* @see		IMsvUniqueWorker
* @see		IMsvThread
* @see		IMsvTask
* @see		MsvThread
* @see		MsvWorker_Factory
******************************************************************************************************/
class MsvUniqueWorker:
	public IMsvUniqueWorker,
	public MsvThread
{
public:
	/**************************************************************************************************//**
	* @brief		Constructor.
	* @param		spFactory		Shared pointer to dependency injection factory.
	* @see		MsvWorker_Factory
	******************************************************************************************************/
	MsvUniqueWorker(std::shared_ptr<MsvWorker_Factory> spFactory = nullptr);

	/**************************************************************************************************//**
	* @brief			Constructor.
	* @details		Constructs @ref MsvUniqueWorker with shared condition variable.
	* @param[in]	spConditionVariable				Shared condition variable.
	* @param[in]	spConditionVariableMutex		Shared condition variable mutex.
	* @param[in]	spConditionVariablePredicate	Shared condition variable predicate.
	* @param[in]	spFactory							Shared pointer to dependency injection factory.
	* @warning		Use this constructor only for workers with shared condition variable. In most cases
	*					you will need default constructor @ref MsvUniqueWorker(std::shared_ptr<MsvWorker_Factory> spFactory).
	* @warning		@ref Notify will notify all threads with shared condition variable.
	* @see			MsvThread
	******************************************************************************************************/
	MsvUniqueWorker(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spConditionVariablePredicate, std::shared_ptr<MsvWorker_Factory> spFactory = nullptr);

	/**************************************************************************************************//**
	* @brief		Virtual destructor.
	******************************************************************************************************/
	virtual ~MsvUniqueWorker();

	/*-----------------------------------------------------------------------------------------------------
	**											IMsvUniqueWorker public methods
	**---------------------------------------------------------------------------------------------------*/
public:
	/**************************************************************************************************//**
	* @copydoc IMsvWorker::AddTask(std::shared_ptr<IMsvTask> spTask)
	******************************************************************************************************/
	virtual MsvErrorCode SetTask(std::shared_ptr<IMsvTask> spTask) override;

	/**************************************************************************************************//**
	* @copydoc IMsvWorker::AddTask(std::function<void()>& task)
	******************************************************************************************************/
	virtual MsvErrorCode SetTask(std::function<void()>& task) override;

	/**************************************************************************************************//**
	* @copydoc IMsvWorker::AddTask(std::function<void(void*)>& task, void* pContext)
	******************************************************************************************************/
	virtual MsvErrorCode SetTask(std::function<void(void*)>& task, void* pContext) override;

	/*-----------------------------------------------------------------------------------------------------
	**											IMsvThread public methods
	**---------------------------------------------------------------------------------------------------*/
public:
	/**************************************************************************************************//**
	* @copydoc IMsvThread::IsRunning()
	******************************************************************************************************/
	virtual bool IsRunning() const override;

	/**************************************************************************************************//**
	* @copydoc MsvThread::Notify()
	******************************************************************************************************/
	virtual void Notify() const override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StartThread()
	******************************************************************************************************/
	virtual MsvErrorCode StartThread(int32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StopThread()
	******************************************************************************************************/
	virtual MsvErrorCode StopThread() override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::StopAndWaitForThreadStop()
	******************************************************************************************************/
	virtual MsvErrorCode StopAndWaitForThreadStop(int32_t timeout) override;

	/**************************************************************************************************//**
	* @copydoc IMsvThread::WaitForThreadStop()
	******************************************************************************************************/
	virtual MsvErrorCode WaitForThreadStop(int32_t timeout) override;

protected:
	/**************************************************************************************************//**
	* @copydoc MsvThread::ThreadMain()
	******************************************************************************************************/
	virtual void ThreadMain() override;

protected:
	/**************************************************************************************************//**
	* @brief		Dependency injection factory.
	* @details	Contains get method for all injected objects.
	* @see		MsvWorker_Factory
	******************************************************************************************************/
	std::shared_ptr<MsvWorker_Factory> m_spFactory;

	/**************************************************************************************************//**
	* @brief		Task lock.
	* @details	Thread safe locking object for setting task (and executing it).
	******************************************************************************************************/
	std::recursive_mutex m_taskLock;

	/**************************************************************************************************//**
	* @brief		Unique task.
	* @details	Contains task/task to execute (will be executed in thread loop).
	******************************************************************************************************/
	std::shared_ptr<IMsvTask> m_spTask;
};


#endif // MARSTECH_UNIQUEWORKER_H

/** @} */	//End of group MTHREADING.
