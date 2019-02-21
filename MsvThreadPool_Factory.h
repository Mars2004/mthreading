/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Thread Pool Factory
* @details		Contains implementation of dependency injection factory @ref MsvThreadPool_Factory
*					for @ref MsvThreadPool.
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


#ifndef MARSTECH_THREADPOOL_FACTORY_H
#define MARSTECH_THREADPOOL_FACTORY_H


#include "mdi/MdiFactory.h"

#include "MsvTask.h"
#include "MsvUniqueWorker.h"

MSV_DISABLE_ALL_WARNINGS

#include <condition_variable>

MSV_ENABLE_WARNINGS


/**************************************************************************************************//**
* @class		MsvThreadPool_Factory
* @brief		MarsTech Thread Pool Dependency Injection Factory.
* @details	Implementation of dependency injection factory for @ref MsvThreadPool.
******************************************************************************************************/
MSV_FACTORY_START(MsvThreadPool_Factory)
MSV_FACTORY_GET_1(IMsvTask, MsvTask, std::function<void()>&);
MSV_FACTORY_GET_2(IMsvTask, MsvTask, std::function<void(void*)>&, void*);
MSV_FACTORY_GET_3(IMsvUniqueWorker, MsvUniqueWorker, std::shared_ptr<std::condition_variable>, std::shared_ptr<std::mutex>, std::shared_ptr<uint64_t>);
MSV_FACTORY_END


#endif // MARSTECH_WORKER_FACTORY_H

/** @} */	//End of group MTHREADING.
