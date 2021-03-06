/*
    Copyright 2016 fishpepper <AT> gmail.com

    This program is free software: you can redistribute it and/ or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http:// www.gnu.org/licenses/>.

    author: fishpepper <AT> gmail.com
*/

#include "storage.h"
#include "debug.h"
#include "wdt.h"
#include "delay.h"
#include "led.h"
#include "frsky.h"
#include "eeprom.h"
#include ".hoptable.h"

// run time copy of persistant storage data:
STORAGE_DESC storage;

void storage_init(void) {
    uint8_t i;

    debug("storage: init\n"); debug_flush();


    // reload data from flash
    storage_load();

    if (storage.version != STORAGE_VERSION_ID) {
        debug("storage: corrupted! bad version\n");
        debug("got 0x");
        debug_put_hex8(storage.version);
        debug_put_newline();
        delay_ms(1000);
        debug_flush();
        storage_load_defaults();
        storage_save();
        // reload to make sure write was ok
        storage_load();
    }

    // for debugging
    // storage_load_defaults();
    // storage_save();


    debug("storage: loaded hoptable[]:\n");
    for (i = 0; i < 9; i++) {
            debug_put_hex8(storage.frsky_hop_table[i]);
            debug_putc(' ');
    }

    debug("...\n");
    debug("storage: txid 0x");
    debug_put_hex8(storage.frsky_txid[0]);
    debug_put_hex8(storage.frsky_txid[1]);
    debug_flush();

    debug("\nstorage: stick calib:\n");
    for (i = 0; i < 4; i++) {
        debug("CH "); debug_put_uint8(i); debug_putc(' ');
        debug_put_uint16(storage.stick_calibration[i][0]); debug_putc('-');
        debug_put_uint16(storage.stick_calibration[i][1]); debug_putc('-');
        debug_put_uint16(storage.stick_calibration[i][2]);
        debug_put_newline();
    }
    debug_flush();
}

static void storage_load_defaults(void) {
    uint8_t i;

    debug("storage: reading defaults\n"); debug_flush();

    static const uint8_t tmp[] = FRSKY_HOPTABLE;

    // set valid version
    storage.version = STORAGE_VERSION_ID;

    // load values from .hoptable.h
    storage.frsky_txid[0] = (FRSYK_TXID>>8) & 0xFF;
    storage.frsky_txid[1] = FRSYK_TXID & 0xFF;

    storage.frsky_freq_offset = FRSKY_DEFAULT_FSCAL_VALUE;

    // copy hoptable
    for (i = 0; i < FRSKY_HOPTABLE_SIZE; i++) {
        storage.frsky_hop_table[i] = tmp[i];
    }

    // stick calib, just dummy values
    for (i = 0; i < 4; i++) {
        storage.stick_calibration[i][0] = 300;
        storage.stick_calibration[i][1] = 2000;
        storage.stick_calibration[i][2] = 4096-300;
    }

    // initialise empty models
    for (i = 0; i < STORAGE_MODEL_MAX_COUNT; i++) {
        storage.model[i].name[0] = 'E';
        storage.model[i].name[1] = 'M';
        storage.model[i].name[2] = 'P';
        storage.model[i].name[3] = 'T';
        storage.model[i].name[4] = 'Y';
        storage.model[i].name[6] = '0' + i;
        storage.model[i].name[6] = 0;
        storage.model[i].timer = 3*60;
        storage.model[i].stick_scale = 100;
    }

    // add example model
    storage_mode_set_name(0, "TinyWhoop");
    storage.model[0].timer = 3*60;
    storage.model[i].stick_scale = 50;
    storage.current_model = 0;
}

void storage_mode_set_name(uint8_t index, uint8_t *str) {
    debug("storage: set modelname ");
    debug_put_uint8(index);
    debug("\n         ");
    debug(str);
    debug_put_newline();
    debug_flush();

    // valid index?
    if (index >= STORAGE_MODEL_MAX_COUNT) {
        // invalid index!
        debug("storage: ERROR invalid index\n");
        debug_flush();
        return;
    }

    // make sure not to exceed the maximum number of chars in name
    uint32_t i;
    for (i = 0; i < STORAGE_MODEL_NAME_LEN; i++) {
        storage.model[index].name[i] = str[i];
        if (str[i] == 0) {
            break;
        }
    }

    // make sure we have a valid zero terminated string in any case
    storage.model[index].name[STORAGE_MODEL_NAME_LEN-1] = 0;
}

void storage_load(void) {
    debug("storage: load\n"); debug_flush();
    eeprom_read_storage();
}

void storage_save(void) {
    debug("storage: save\n"); debug_flush();
    eeprom_write_storage();
}


static void storage_write(uint8_t *buffer, uint16_t len) {
}

static void storage_read(uint8_t *storage_ptr, uint16_t len) {
}


