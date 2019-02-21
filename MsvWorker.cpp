/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Worker Implementation
* @details		Contains implementation of @ref MsvWorker.
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


#include "MsvWorker.h"
#include "MsvWorker_Factory.h"

#include "MsvTask.h"

#include "merror/MsvErrorCodes.h"


/********************************************************************************************************************************
*                                              Constructors and destructors
********************************************************************************************************************************/


MsvWorker::MsvWorker(std::shared_ptr<MsvWorker_Factory> spFactory):
	MsvWorker(nullptr, nullptr, nullptr, spFactory)
{

}

MsvWorker::MsvWorker(std::shared_ptr<std::condition_variable> spConditionVariable, std::shared_ptr<std::mutex> spConditionVariableMutex, std::shared_ptr<uint64_t> spSharedConditionPredicate, std::shared_ptr<MsvWorker_Factory> spFactory):
	MsvThread(spConditionVariable, spConditionVariableMutex, spSharedConditionPredicate),
	m_spFactory(spFactory ? spFactory : MsvWorker_Factory::Get())
{
	
}

MsvWorker::~MsvWorker()
{
}


/********************************************************************************************************************************
*                                              IMsvWorker public methods
********************************************************************************************************************************/


void MsvWorker::AddTask(std::shared_ptr<IMsvTask> spTask)
{
	std::lock_guard<std::recursive_mutex> lock(m_taskLock);
	m_tasks.push(spTask);

	Notify();
}

MsvErrorCode MsvWorker::AddTask(std::function<void()>& task)
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

MsvErrorCode MsvWorker::AddTask(std::function<void(void*)>& task, void* pContext)
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


/********************************************************************************************************************************
*															IMsvThread public methods
********************************************************************************************************************************/


bool MsvWorker::IsRunning() const
{
	return MsvThread::IsRunning();
}

void MsvWorker::Notify() const
{
	MsvThread::Notify();
}

MsvErrorCode MsvWorker::StartThread(int32_t timeout)
{
	return MsvThread::StartThread(timeout);
}

MsvErrorCode MsvWorker::StopThread()
{
	return MsvThread::StopThread();
}

MsvErrorCode MsvWorker::StopAndWaitForThreadStop(int32_t timeout)
{
	return MsvThread::StopAndWaitForThreadStop(timeout);
}

MsvErrorCode MsvWorker::WaitForThreadStop(int32_t timeout)
{
	return MsvThread::WaitForThreadStop(timeout);
}


/********************************************************************************************************************************
*                                              MsvThread protected methods
********************************************************************************************************************************/


void MsvWorker::ThreadMain()
{
	std::unique_lock<std::recursive_mutex> lock(m_taskLock);

	while (!m_tasks.empty())
	{
		//get current task
		std::shared_ptr<IMsvTask> spTask = m_tasks.front();
		m_tasks.pop();

		if (!spTask)
		{
			//task is not valid -> skip to next
			continue;
		}

		//unlock (anyone can add new task during current task execution)
		lock.unlock();
		spTask->Execute();

		//lock because of next loop condition execution (and front() and pop())
		lock.lock();
	}
}


/** @} */	//End of group MTHREADING.
