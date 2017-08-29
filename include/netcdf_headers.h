/*
  Copyright (C) 2014-2017 Sven Willner <sven.willner@pik-potsdam.de>
                          Christian Otto <christian.otto@pik-potsdam.de>

  This file is part of Acclimate.

  Acclimate is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  Acclimate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with Acclimate.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ACCLIMATE_NETCDF_HEADERS_H
#define ACCLIMATE_NETCDF_HEADERS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <ncDim.h>
#include <ncFile.h>
#include <ncGroupAtt.h>
#include <ncType.h>
#include <ncVar.h>
#include <netcdf>
#pragma GCC diagnostic pop

namespace netCDF {
class UNUSED_file_id_class {
    static void UNUSED_file_id() {  // prevent error from compiler
        (void)(netCDF::file_id);
    }
};
}  // namespace netCDF

using namespace netCDF;

#endif