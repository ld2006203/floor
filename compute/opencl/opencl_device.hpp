/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2014 Florian Ziesche
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License only.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __FLOOR_OPENCL_DEVICE_HPP__
#define __FLOOR_OPENCL_DEVICE_HPP__

#include <floor/core/essentials.hpp>

#if !defined(FLOOR_NO_OPENCL)

#include <floor/compute/compute_device.hpp>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

enum class OPENCL_VERSION : uint32_t;

class opencl_device final : public compute_device {
public:
	~opencl_device() override {}
	
	//! opencl c version of the device
	OPENCL_VERSION c_version;
	
};

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif

#endif
