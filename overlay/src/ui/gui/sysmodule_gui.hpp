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

class SysmodulesGui : public tsl::Gui {
public:
    SysmodulesGui() : m_tableBuf(nullptr), m_tableSize(0) {}

    ~SysmodulesGui() {
        if (m_tableBuf) {
            const size_t entryCount = m_tableSize / sizeof(Sysmodule);
            Sysmodule *entries = reinterpret_cast<Sysmodule *>(m_tableBuf);
            for (size_t i = 0; i < entryCount; i++)
                entries[i].~Sysmodule();
            delete[] m_tableBuf;
        }
    }

    tsl::elm::Element *createUI() override {
        auto *frame = new tsl::elm::OverlayFrame("Sysmodules", "");
        auto *list  = new tsl::elm::List();

        Result rc = IpcGetSysmoduleTableSize(&m_tableSize);
        if (R_FAILED(rc) || m_tableSize == 0) {
            char errMsg[64];
            snprintf(errMsg, sizeof(errMsg), "rc=0x%x size=%zu", rc, m_tableSize);
            list->addItem(new tsl::elm::ListItem(errMsg));
            frame->setContent(list);
            return frame;
        }

        const size_t entryCount = m_tableSize / sizeof(Sysmodule);
        m_tableBuf = new u8[m_tableSize];
        Sysmodule *entries = reinterpret_cast<Sysmodule *>(m_tableBuf);
        for (size_t i = 0; i < entryCount; i++)
            new (&entries[i]) Sysmodule{};

        rc = IpcGetSysmodulesTable(entries, m_tableSize);
        if (R_FAILED(rc)) {
            char errMsg[64];
            snprintf(errMsg, sizeof(errMsg), "table rc=0x%x", rc);
            list->addItem(new tsl::elm::ListItem(errMsg));
            frame->setContent(list);
            return frame;
        }

        for (u32 i = 0; i < static_cast<u32>(entryCount); i++) {
            const Sysmodule &entry = entries[i];

            char idStr[20];
            snprintf(idStr, sizeof(idStr), "0x%llx", (unsigned long long)entry.id);

            auto *item = new tsl::elm::ListItem(entry.name);
            item->setTextColor(tsl::Color(120, 255, 160, 255));
            item->setValue(idStr, true);
            item->setValueColor(tsl::Color(255, 210, 80, 255));

            const u64 entryId = entry.id;
            item->setClickListener([entryId](u64 keys) -> bool {
                if (keys & HidNpadButton_A) {
                    DumpSysmoduleHeap(entryId);
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
