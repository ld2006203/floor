/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2015 Florian Ziesche
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

#include <floor/compute/metal/metal_program.hpp>

#if !defined(FLOOR_NO_METAL)

#include <floor/compute/metal/metal_kernel.hpp>
#include <floor/compute/llvm_compute.hpp>
#include <floor/core/core.hpp>

metal_program::metal_program(const metal_device* device,
							 id <MTLLibrary> program_,
							 const vector<llvm_compute::kernel_info>& kernels_info) : program(program_) {
	// create kernels (all in the program)
	const auto kernel_count = kernels_info.size();
	metal_kernels.resize(kernel_count);
	for(size_t i = 0; i < kernel_count; ++i) {
		const auto& info = kernels_info[i];
		id <MTLFunction> kernel = [program newFunctionWithName:[NSString stringWithUTF8String:info.name.c_str()]];
		if(!kernel) {
			log_error("failed to get function \"%s\"", info.name);
			continue;
		}
#if defined(FLOOR_DEBUG)
		log_debug("kernel func info: %s, %u", [[kernel name] UTF8String], (int)[kernel functionType]);
		logger::flush();
#endif
		
		NSError* err = nullptr;
		id <MTLComputePipelineState> kernel_state = [[program device] newComputePipelineStateWithFunction:kernel error:&err];
		if(!kernel_state) {
			log_error("failed to create kernel state %s: %s", info.name,
					  (err != nullptr ? [[err localizedDescription] UTF8String] : "unknown error"));
			continue;
		}
#if defined(FLOOR_DEBUG)
		log_debug("%s: max work-items: %u, simd width: %u",
				  info.name, [kernel_state maxTotalThreadsPerThreadgroup], [kernel_state threadExecutionWidth]);
#endif
		
		metal_kernels[i].kernel = kernel;
		metal_kernels[i].state = kernel_state;
		kernels.emplace_back(make_shared<metal_kernel>((__bridge void*)metal_kernels[i].kernel,
													   (__bridge void*)metal_kernels[i].state,
													   device, info));
		kernel_names.emplace_back(info.name);
	}
}

#endif
