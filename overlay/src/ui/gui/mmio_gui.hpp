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
#include <tesla.hpp>
#include <cstdio>
#include <new>
#include "../../ipc.hpp"

class MmioGui : public tsl::Gui {
public:
    MmioGui() : m_tableBuf(nullptr), m_tableSize(0) {}

    ~MmioGui() {
        if (m_tableBuf) {
            const size_t entryCount = m_tableSize / sizeof(MmioTableEntry);
            MmioTableEntry *entries = reinterpret_cast<MmioTableEntry *>(m_tableBuf);
            for (size_t i = 0; i < entryCount; i++)
                entries[i].~MmioTableEntry();
            delete[] m_tableBuf;
        }
    }

    tsl::elm::Element *createUI() override {
        auto *frame = new tsl::elm::OverlayFrame("MMIO Regions", "");
        auto *list  = new tsl::elm::List();

        Result rc = IpcGetMmioTableSize(&m_tableSize);
        if (R_FAILED(rc) || m_tableSize == 0) {
            char errMsg[64];
            snprintf(errMsg, sizeof(errMsg), "rc=0x%x size=%zu", rc, m_tableSize);
            list->addItem(new tsl::elm::ListItem(errMsg));
            frame->setContent(list);
            return frame;
        }

        const size_t entryCount = m_tableSize / sizeof(MmioTableEntry);
        m_tableBuf = new u8[m_tableSize];
        MmioTableEntry *entries = reinterpret_cast<MmioTableEntry *>(m_tableBuf);
        for (size_t i = 0; i < entryCount; i++)
            new (&entries[i]) MmioTableEntry{};

        rc = IpcGetMmioTable(entries, m_tableSize);
        if (R_FAILED(rc)) {
            char errMsg[64];
            snprintf(errMsg, sizeof(errMsg), "table rc=0x%x", rc);
            list->addItem(new tsl::elm::ListItem(errMsg));
            frame->setContent(list);
            return frame;
        }

        for (u32 i = 0; i < static_cast<u32>(entryCount); i++) {
            const MmioTableEntry &entry = entries[i];

            char addrStr[24];
            snprintf(addrStr, sizeof(addrStr), "0x%llx", (unsigned long long)entry.address);

            auto *item = new tsl::elm::ListItem(entry.name);
            item->setTextColor(tsl::Color(120, 235, 255, 255));
            item->setValue(addrStr, true);
            item->setValueColor(tsl::Color(255, 210, 80, 255));

            const u32 idx = i;
            item->setClickListener([idx](u64 keys) -> bool {
                if (keys & HidNpadButton_A) {
                    IpcDumpMmio(idx);
                    return true;
                }
                return false;
            });
            list->addItem(item);
        }

        frame->setContent(list);
        return frame;
    }

    void update() override {}

private:
    u8     *m_tableBuf;
    size_t  m_tableSize;
};
