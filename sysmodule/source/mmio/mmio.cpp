/*
 * Copyright (c) Lightos_
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <switch.h>
#include <cstring>
#include <memdump.hpp>
#include "../fs/fs.hpp"
#include "mmio_table.hpp"

namespace mmio {

    Result MapMemory(u64 &virtualAddr, u64 physAddr, size_t size) {
        u64 outSize = 0;
        fs::Log("In physAddr: %ull", physAddr);
        fs::Log("In size: %zu", size);
        Result rs = svcQueryMemoryMapping(&virtualAddr, &outSize, physAddr, size);
        fs::Log("Outsize: %ull", outSize);
        R_RETURN(rs);
    }

    Result SecmonReadRegister(u32 *out, u64 physAddr, u32 offset) {
        SecmonArgs args = {};
        args.X[0]       = 0xF0000002;
        u64 address     = physAddr + offset;
        args.X[1]       = address;

        svcCallSecureMonitor(&args);

        /* svcCallSecureMonitor always returns ResultSuccess in modern firmware, so this shit is required. */
        if (args.X[1] == address) {
            return 1;
        }

        out[offset] = args.X[1];

        R_SUCCEED();
    }

    Result SecmonReadRegion(u8 *out, u64 physAddr, size_t size) {
        /* smcReadWrite register operates on u32. */
        size_t registerSize = size / sizeof(u32);
        u32 *bufferU32      = new u32[registerSize];

        for (size_t i = 0; i < registerSize; ++i) {
            Result rs = SecmonReadRegister(bufferU32, physAddr, i);

            /* failure, clean and exit. */
            if (R_FAILED(rs)) {
                delete[] bufferU32;
                return rs;
            }
        }

        /* Store back into u8 buffer and cleanup. */
        memcpy(out, bufferU32, size);
        delete[] bufferU32;

        R_SUCCEED();
    }

    Result DumpRegion(u8 *out, u64 physAddr, size_t size) {
        u64 virtualAddr = 0;
        if (R_FAILED(MapMemory(virtualAddr, physAddr, size))) {
            /* Mapping fails, try to read via secmon. */
            return SecmonReadRegion(out, physAddr, size);
        }

        /* Copy mapped memory. */
        memcpy(out, reinterpret_cast<void *>(virtualAddr), size);
        R_SUCCEED();
    }

    Result DumpRegionIdx(u32 idx) {
        fs::Log("Got to DumpRegionIdx, %u", idx);
        MmioTableEntry entry = mmioTable[idx];
        u8 *buffer           = new u8[entry.size];
        Result rs            = 0;

        do {
            rs = DumpRegion(buffer, entry.address, entry.size);

            if (R_FAILED(rs)) {
                break;
            }

            fs::WriteBinDump(buffer, entry.size, entry.name);
        } while (false);

        delete[] buffer;
        return rs;
    }

}
