/**************************************************************************************************//**
* @addtogroup	MTHREADING
* @{
******************************************************************************************************/

/**************************************************************************************************//**
* @file
* @brief			MarsTech Worker Factory
* @details		Contains implementation of dependency injection factory @ref MsvWorker_Factory
*					for @ref MsvWorker and for @ref MsvUniqueWorker.
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


#ifndef MARSTECH_WORKER_FACTORY_H
#define MARSTECH_WORKER_FACTORY_H


#include "mdi/MdiFactory.h"

#include "MsvTask.h"


/**************************************************************************************************//**
* @class		MsvWorker_Factory
* @brief		MarsTech Worker Dependency Injection Factory.
* @details	Implementation of dependency injection factory for @ref MsvWorker.
******************************************************************************************************/
MSV_FACTORY_START(MsvWorker_Factory)
	MSV_FACTORY_GET_1(IMsvTask, MsvTask, std::function<void()>&);
	MSV_FACTORY_GET_2(IMsvTask, MsvTask, std::function<void(void*)>&, void*);
MSV_FACTORY_END


#endif // MARSTECH_WORKER_FACTORY_H

/** @} */	//End of group MTHREADING.
