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
#include "mmio_gui.hpp"
#include "sysmodule_gui.hpp"

class MainGui : public tsl::Gui {
public:
    MainGui() {}

    tsl::elm::Element *createUI() override {
        auto *frame = new tsl::elm::OverlayFrame("nx-memdump", "");
        auto *list  = new tsl::elm::List();

        auto *mmioItem = new tsl::elm::ListItem("MMIO Regions");
        mmioItem->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<MmioGui>();
                return true;
            }
            return false;
        });
        list->addItem(mmioItem);

        auto *sysItem = new tsl::elm::ListItem("Sysmodules");
        sysItem->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SysmodulesGui>();
                return true;
            }
            return false;
        });
        list->addItem(sysItem);

        frame->setContent(list);
        return frame;
    }

    void update() override {}
};
