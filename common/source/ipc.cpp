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

#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <switch.h>
#include <string.h>
#include <stdatomic.h>
#include <memdump.hpp>

static Service service;
static atomic_size_t g_refCnt;

bool IpcRunning() {
    Handle handle;
    bool running = R_FAILED(smRegisterService(&handle, smEncodeName(IPC_SERVICE_NAME), false, 1));

    if (!running) {
        smUnregisterService(smEncodeName(IPC_SERVICE_NAME));
    }

    return running;
}

Result IpcInitialize(void) {
    Result rc = 0;

    g_refCnt++;

    if (serviceIsActive(&service)) {
        return 0;
    }

    rc = smGetService(&service, IPC_SERVICE_NAME);

    if (R_FAILED(rc)) {
        IpcExit();
    }

    return rc;
}

void IpcExit(void) {
    if (--g_refCnt == 0) {
        serviceClose(&service);
    }
}

Result IpcGetMmioTableSize(size_t *outSize) {
    return serviceDispatchOut(&service, IpcCmd_GetMmioTableSize, *outSize);
}

Result IpcGetSysmoduleTableSize(size_t *outSize) {
    return serviceDispatchOut(&service, IpcCmd_GetSysmodulesTableSize, *outSize);
}

Result IpcGetMmioTable(MmioTableEntry *mmioTable, size_t mmioTableSize) {
    return serviceDispatch(&service, IpcCmd_GetMmioTable,
        .buffer_attrs = { SfBufferAttr_HipcAutoSelect | SfBufferAttr_Out },
        .buffers = {{mmioTable, mmioTableSize}},
    );
}

Result IpcGetSysmodulesTable(Sysmodule *sysmodules, size_t size) {
    return serviceDispatch(&service, IpcCmd_GetSysmodulesTable,
        .buffer_attrs = { SfBufferAttr_HipcAutoSelect | SfBufferAttr_Out },
        .buffers = {{sysmodules, size}},
    );
}

Result DumpSysmoduleHeap(u64 id) {
    return serviceDispatchIn(&service, IpcCmd_DumpSysmoduleHeap, id);
}

Result IpcDumpMmio(u32 idx) {
    return serviceDispatchIn(&service, IpcCmd_DumpMmio, idx);
}
