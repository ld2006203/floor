/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2017 Florian Ziesche
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

#ifndef __FLOOR_CUDA_DEVICE_HPP__
#define __FLOOR_CUDA_DEVICE_HPP__

#include <floor/compute/cuda/cuda_common.hpp>
#include <floor/compute/compute_device.hpp>

FLOOR_PUSH_WARNINGS()
FLOOR_IGNORE_WARNING(weak-vtables)

class cuda_compute;
class cuda_device final : public compute_device {
public:
	cuda_device();
	~cuda_device() override {}
	
	//! compute capability (aka sm_xx)
	uint2 sm { 2, 0 };
	
	//!
	uint32_t max_registers_per_block { 0u };
	
	//!
	uint32_t l2_cache_size { 0u };
	
	//!
	uint32_t vendor_id { 0u };
	
	//!
	uint32_t warp_size { 0u };
	
	//!
	uint32_t mem_bus_width { 0u };
	
	//!
	uint32_t async_engine_count { 0u };
	
#if !defined(FLOOR_NO_CUDA)
	//! cuda requires a context for each device (no shared context)
	cu_context ctx { nullptr };
	
	//!
	cu_device device_id { 0u };
	
	//! the original device sampler init function pointer
	CU_API CU_RESULT (*sampler_init_func_ptr)(cu_texture_ref tex_ref) { nullptr };
#else
	void* _ctx { nullptr };
	int32_t _device_id { 0u };
	void* _sampler_init_func_ptr { nullptr };
#endif
	
};

FLOOR_POP_WARNINGS()

#endif
