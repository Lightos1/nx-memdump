/* This ipc stuff was copy pasted from hoc/sys-clk */

/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
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
 *
 */

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <switch.h>
#include <nxExt.h>
#include <memdump.hpp>
#include "../mmio/mmio_table.hpp"
#include "../mmio/mmio.hpp"
#include "../fs/fs.hpp"
#include "../sys/sysmodules.hpp"

namespace ipc {

    bool gRunning = false;
    Thread gThread;
    LockableMutex gThreadMutex;
    IpcServer gServer;

    inline bool EnsureBuffer(size_t required, size_t *sizeOut) {
        if (*sizeOut < required) {
            *sizeOut = required;
            return false;
        }
        return true;
    }

    Result ServiceHandlerFunc(void *arg, const IpcServerRequest *r, u8 *out_data, size_t *out_dataSize) {
        (void) arg;

        switch (r->data.cmdId) {
            case IpcCmd_GetMmioTableSize:
                *out_dataSize = sizeof(size_t);
                fs::Log("size: %u", sizeof(mmio::mmioTable));
                return mmio::GetMmioTableSize(reinterpret_cast<size_t *>(out_data));
            case IpcCmd_GetMmioTable:
                if (r->hipc.meta.num_recv_buffers >= 1) {
                    size_t bufSize = hipcGetBufferSize(r->hipc.data.recv_buffers);
                    if (bufSize >= sizeof(mmio::mmioTable)) {
                        return mmio::GetMmioTable(reinterpret_cast<MmioTableEntry *>(hipcGetBufferAddress(r->hipc.data.recv_buffers)));
                    }
                }
                break;
            case IpcCmd_GetSysmodulesTableSize:
                *out_dataSize = sizeof(size_t);
                return sys::GetSysTableSize(reinterpret_cast<size_t *>(out_data));
            case IpcCmd_GetSysmodulesTable:
                if (r->hipc.meta.num_recv_buffers >= 1) {
                    size_t bufSize = hipcGetBufferSize(r->hipc.data.recv_buffers);
                    if (bufSize >= sizeof(sys::sysmodules)) {
                        return sys::GetSysmodules(reinterpret_cast<Sysmodule *>(hipcGetBufferAddress(r->hipc.data.recv_buffers)));
                    }
                }
                break;
            case IpcCmd_DumpMmio:
                if (r->data.size >= sizeof(u32)) {
                    u32 idx;
                    memcpy(&idx, r->data.ptr, sizeof(u32));
                    return mmio::DumpRegionIdx(idx);
                }
                break;
            case IpcCmd_DumpSysmoduleHeap:
                if (r->data.size >= sizeof(u64)) {
                    u64 id;
                    memcpy(&id, r->data.ptr, sizeof(u64));
                    return sys::DumpSysmodule(id);
                }
                break;
            default:
            break;
        }

        return 1;
    }

    void ProcessThreadFunc(void* arg) {
        (void)arg;
        Result rc;
        while (true) {
            rc = ipcServerProcess(&gServer, &ServiceHandlerFunc, nullptr);
            if (R_FAILED(rc)) {
                if (rc == KERNELRESULT(Cancelled)) {
                    return;
                }
                if (rc != KERNELRESULT(ConnectionClosed)) {
                    fs::Log("[ipc] ipcServerProcess: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
                }
            }
        }
    }

    void Initialize() {
        Result rc;

        s32 priority;
        rc = svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
        if (R_FAILED(rc)) {
            fs::Log("[ipc] svcGetThreadPriority failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
            return;
        }

        rc = ipcServerInit(&gServer, IPC_SERVICE_NAME, 42);
        if (R_FAILED(rc)) {
            fs::Log("[ipc] ipcServerInit failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
            return;
        }

        rc = threadCreate(&gThread, &ProcessThreadFunc, nullptr, NULL, 0x4000, priority, -2);
        if (R_FAILED(rc)) {
            fs::Log("[ipc] threadCreate failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
            ipcServerExit(&gServer);
            return;
        }

        fs::Log("[ipc] Initialized successfully (priority=%d)", priority);
        gRunning = false;
    }

    void SetRunning(bool running) {
        std::scoped_lock lock{gThreadMutex};
        if (gRunning == running) {
            return;
        }
        gRunning = running;
        if (running) {
            Result rc = threadStart(&gThread);
            if (R_FAILED(rc)) {
                fs::Log("[ipc] threadStart failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
                gRunning = false;
                return;
            }
            fs::Log("[ipc] Thread started");
        } else {
            fs::Log("[ipc] Stopping thread...");
            svcCancelSynchronization(gThread.handle);
            threadWaitForExit(&gThread);
            fs::Log("[ipc] Thread stopped");
        }
    }

    void Exit() {
        SetRunning(false);

        Result rc = threadClose(&gThread);
        if (R_FAILED(rc)) {
            fs::Log("[ipc] threadClose failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
        }

        rc = ipcServerExit(&gServer);
        if (R_FAILED(rc)) {
            fs::Log("[ipc] ipcServerExit failed: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
        }

        fs::Log("[ipc] Exited");
    }
}
