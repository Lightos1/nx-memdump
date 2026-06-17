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

#pragma once
#include <switch.h>
#include <memdump.hpp>
#include <string>
#include <cstring>

namespace sys {

    /* List sourced from https://gist.github.com/ndeadly/a4b8c01bb453028cd0008f282098f696, thanks! */
    const Sysmodule sysmodules[] = {
        { "Fs", 0x0100000000000000, },
        { "Ldr", 0x0100000000000001, },
        { "Ncm", 0x0100000000000002, },
        { "Pm", 0x0100000000000003, },
        { "Sm", 0x0100000000000004, },
        { "Boot", 0x0100000000000005, },
        { "Usb", 0x0100000000000006, },
        { "Tma_HtcStub", 0x0100000000000007, },
        { "Boot2", 0x0100000000000008, },
        { "Settings", 0x0100000000000009, },
        { "Bus", 0x010000000000000A, },
        { "Bluetooth", 0x010000000000000B, },
        { "Bcat", 0x010000000000000C, },
        { "Dmnt", 0x010000000000000D, },
        { "Friends", 0x010000000000000E, },
        { "Nifm", 0x010000000000000F, },
        { "Ptm", 0x0100000000000010, },
        { "Shell", 0x0100000000000011, },
        { "Bsdsockets", 0x0100000000000012, },
        { "Hid", 0x0100000000000013, },
        { "Audio", 0x0100000000000014, },
        { "LogManager", 0x0100000000000015, },
        { "Wlan", 0x0100000000000016, },
        { "Cs", 0x0100000000000017, },
        { "Ldn", 0x0100000000000018, },
        { "Nvservices", 0x0100000000000019, },
        { "Pcv", 0x010000000000001A, },
        { "Ppc_Capmtp", 0x010000000000001B, },
        { "Nvnflinger", 0x010000000000001C, },
        { "Pcie", 0x010000000000001D, },
        { "Account", 0x010000000000001E, },
        { "Ns", 0x010000000000001F, },
        { "Nfc", 0x0100000000000020, },
        { "Psc", 0x0100000000000021, },
        { "Capsrv", 0x0100000000000022, },
        { "Am", 0x0100000000000023, },
        { "Ssl", 0x0100000000000024, },
        { "Nim", 0x0100000000000025, },
        { "Cec", 0x0100000000000026, },
        { "Tspm", 0x0100000000000027, },
        { "Spl", 0x0100000000000028, },
        { "Lbl", 0x0100000000000029, },
        { "Btm", 0x010000000000002A, },
        { "Erpt", 0x010000000000002B, },
        { "Time", 0x010000000000002C, },
        { "Vi", 0x010000000000002D, },
        { "Pctl", 0x010000000000002E, },
        { "Npns", 0x010000000000002F, },
        { "Eupld", 0x0100000000000030, },
        { "Arp_Glue", 0x0100000000000031, },
        { "Eclct", 0x0100000000000032, },
        { "Es", 0x0100000000000033, },
        { "Fatal", 0x0100000000000034, },
        { "Grc", 0x0100000000000035, },
        { "Creport", 0x0100000000000036, },
        { "Ro", 0x0100000000000037, },
        { "Profiler", 0x0100000000000038, },
        { "Sdb", 0x0100000000000039, },
        { "Migration", 0x010000000000003A, },
        { "Jit", 0x010000000000003B, },
        { "Jpegdec", 0x010000000000003C, },
        { "Safemode", 0x010000000000003D, },
        { "Olsc", 0x010000000000003E, },
        { "Dt", 0x010000000000003F, },
        { "Nd", 0x0100000000000040, },
        { "Ngct", 0x0100000000000041, },
        { "Pgl", 0x0100000000000042, },
        { "Omm", 0x0100000000000045, },
        { "Eth", 0x0100000000000046, },
        { "Ngc", 0x0100000000000050, },
        { "Dmmt", 0x010000000000D609, },
        { "AmsMitm", 0x010041544d530000, },

        /* Applets. */
        { "Qlaunch", 0x0100000000001000 },
        { "Auth", 0x0100000000001001 },
        { "Cabinet", 0x0100000000001002 },
        { "Controller", 0x0100000000001003 },
        { "DataErase", 0x0100000000001004 },
        { "Error", 0x0100000000001005 },
        { "NetConnect", 0x0100000000001006 },
        { "PlayerSelect", 0x0100000000001007 },
        { "Swkbd", 0x0100000000001008 },
        { "MiiEdit", 0x0100000000001009 },
        { "LibAppletWeb", 0x010000000000100A },
        { "LibAppletShop", 0x010000000000100B },
        { "OverlayDisp", 0x010000000000100C },
        { "PhotoViewer", 0x010000000000100D },
        { "LibAppletOff", 0x010000000000100F },
        { "LibAppletLns", 0x0100000000001010 },
        { "LibAppletAuth", 0x0100000000001011 },
        { "Starter", 0x0100000000001012 },
        { "MyPage", 0x0100000000001013 },
        { "Maintenance", 0x0100000000001015 },
        { "SystemWeb", 0x0100000000001042 },
        { "OpenWeb", 0x0100000000001043 },
        { "Splay", 0x0100000000001048 },
    };

    inline Result GetSysTableSize(size_t *size) {
        *size = sizeof(sysmodules);
        R_SUCCEED();
    }


    inline Result GetSysmodules(Sysmodule *sysmoduleBuffer) {
        std::memcpy(sysmoduleBuffer, sysmodules, sizeof(sysmodules));
        R_SUCCEED();
    }


    Result DumpSysmodule(u64 id);

}
