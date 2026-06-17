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

#pragma once
#include <switch.h>
#include "mmio.hpp"
#include "sys.hpp"

#define IPC_SERVICE_NAME "mem:dmp"
enum IpcCmd {
    IpcCmd_GetMmioTableSize         = 0,
    IpcCmd_GetMmioTable             = 1,
    IpcCmd_GetSysmodulesTableSize   = 2,
    IpcCmd_GetSysmodulesTable       = 3,
    IpcCmd_DumpMmio                 = 4,
    IpcCmd_DumpSysmoduleHeap        = 5,
};

bool IpcRunning();
Result IpcInitialize(void);
void IpcExit(void);

Result IpcGetMmioTableSize(size_t *outSize);
Result IpcGetMmioTable(MmioTableEntry *mmioTable, size_t size);
Result IpcGetSysmoduleTableSize(size_t *outSize);
Result IpcGetSysmodulesTable(Sysmodule *sysmodules, size_t size);
Result IpcDumpMmio(u32 index);
Result DumpSysmoduleHeap(u64 id);
