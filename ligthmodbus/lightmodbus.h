/*
    Copyright (C) 2021 Jacek Wieczorek <mrjjot@gmail.com>
 
    This file is part of liblightmodbus.
 
    Liblightmodbus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    Liblightmodbus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
 
// For C++
#ifdef __cplusplus
extern "C" {
#endif
 
#ifndef LIGHTMODBUS_H
#define LIGHTMODBUS_H

#define LIGHTMODBUS_SLAVE
 
/*
    Optionally include a configuration file here
*/
#ifdef LIGHTMODBUS_USE_CONFIG_FILE
    #ifdef LIGHTMODBUS_CONFIG_FILE
        #include LIGHTMODBUS_CONFIG_FILE
    #else
        #include "config.h"
    #endif
#endif
 
#ifdef LIGHTMODBUS_FULL
    #ifndef LIGHTMODBUS_SLAVE_FULL
    #define LIGHTMODBUS_SLAVE_FULL
    #endif
 
    #ifndef LIGHTMODBUS_MASTER_FULL
    #define LIGHTMODBUS_MASTER_FULL
    #endif
#endif
 
#ifdef LIGHTMODBUS_SLAVE_FULL
    #ifndef LIGHTMODBUS_SLAVE
    #define LIGHTMODBUS_SLAVE
    #endif
#endif
 
#ifdef LIGHTMODBUS_MASTER_FULL
    #ifndef LIGHTMODBUS_MASTER 
    #define LIGHTMODBUS_MASTER
    #endif
#endif
#endif
 
// Always include base
#include "base.h"
 
#ifdef LIGHTMODBUS_SLAVE
    #include "slave.h"
    #include "slave_func.h"
#endif
 
#ifdef LIGHTMODBUS_MASTER
    #include "master.h"
    #include "master_func.h"
#endif
 
#ifdef LIGHTMODBUS_DEBUG
    #include "debug.h"
#endif
 
#ifdef LIGHTMODBUS_IMPL
    #include "base.impl.h"
    #ifdef LIGHTMODBUS_SLAVE
        #include "slave.impl.h"
        #include "slave_func.impl.h"
    #endif
 
    #ifdef LIGHTMODBUS_MASTER
        #include "master.impl.h"
        #include "master_func.impl.h"
    #endif
 
    #ifdef LIGHTMODBUS_DEBUG
        #include "debug.impl.h"
    #endif
#endif
 
// For C++ (closes `extern "C"` )
#ifdef __cplusplus
}
#endif