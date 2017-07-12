// Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** \file audiodata.h
 * Contains definitions for USB audio communication between the FX2 chip and
 * the host
 */

#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

#include <audio.h>

#include <fx2types.h>

BYTE handle_get_configuration();
BOOL handle_set_configuration(BYTE cfg);
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc);
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc);
BOOL handle_get_descriptor();

#endif // AUDIO_DATA_H
