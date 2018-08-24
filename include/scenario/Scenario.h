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

#ifndef ACCLIMATE_SCENARIO_H
#define ACCLIMATE_SCENARIO_H

#include <string>
#include "model/Consumer.h"
#include "model/Firm.h"
#include "model/Model.h"
#include "settingsnode.h"
#include "types.h"

namespace acclimate {

template<class ModelVariant>
class Scenario {
  protected:
    settings::SettingsNode scenario_node;
    const settings::SettingsNode& settings;
    Model<ModelVariant>* const model_m;

  public:
    Scenario(const settings::SettingsNode& settings_p, settings::SettingsNode scenario_node_p, Model<ModelVariant>* const model_p);
    virtual ~Scenario() {}
    virtual Time start() { return Time(0.0); }  // TODO eliminate return type
    virtual Time start_time() { return model()->start_time();};
    virtual void end() {}
    virtual bool iterate() {};
    virtual bool is_first_timestep() const { return model()->timestep() == 0; }
    virtual bool is_last_timestep() const { return model()->time() >= model()->stop_time(); }
    virtual std::string calendar_str() const { return "standard"; }
    virtual std::string time_units_str() const;
    inline Model<ModelVariant>* model() const { return model_m; }
    virtual inline std::string id() const { return "SCENARIO"; }
};
}  // namespace acclimate

#endif
