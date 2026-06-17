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

#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "ui/gui/main_gui.hpp"
#include "ipc.hpp"

class MemDumpOverlay : public tsl::Overlay {
public:
    void initServices() override {
        IpcInitialize();
    }

    void exitServices() override {
        IpcExit();
    }

    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return std::make_unique<MainGui>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<MemDumpOverlay>(argc, argv);
}
