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

#include "scenario/ScenarioController.h"
#include <random>
#include <algorithm>
#include <utility>
#include "model/EconomicAgent.h"
#include "model/Region.h"
#include "model/Sector.h"
#include "run.h"
#include "settingsnode.h"
#include "variants/ModelVariants.h"

namespace acclimate {

template<class ModelVariant>
ScenarioController<ModelVariant>::ScenarioController() : {}


template<class ModelVariant>
void ScenarioController<ModelVariant>::start() {
    Time scenarios_start_time;
    bool boolean_first_scenario = true;
    for (const auto& scenario : scenarios) {
        Time start_time = scenario->start_time();
        if ( boolean_first_scenario || start_time < scenarios_start_time ) {
            scenarios_start_time = start_time;
            boolean_first_scenario = false;
        }
    }
    if (scenarios_start_time < model()->start_time()) {
        model()->start_time(scenarios_start_time);
    }
}

template<class ModelVariant>
void ScenarioController<ModelVariant>::iterate() {
    for (auto& s : model()->sectors) {
        for (auto& firm : s->firms) {
            firm->forcing(1.0);
        }
    }
    for (auto& r : model()->regions) {
        find_consumer(r)->forcing(1.0);
    }
    for (auto& location : model()->other_locations) {
        location->set_forcing_nu(-1.);
    }

    for (const auto& scenario : scenarios ) {
        scenario->iterate();
    }
}


template<class ModelVariant>
void ScenarioController<ModelVariant>::set_firm_forcing(Firm<ModelVariant>* firm, const Forcing& forcing) {
    if (forcing < firm->forcing()) {
        firm->forcing(forcing);
    }
}

template<class ModelVariant>
void ScenarioController<ModelVariant>::set_location_forcing(GeoLocation<ModelVariant>* location, const Forcing& forcing) {
    if (location < 0.) {
        location->set_forcing_nu(forcing);
    } else {
        if ( forcing > 0. && location->forcing() > forcing  ) {
            location->set_forcing_nu(forcing);
        }
    }
}

template<class ModelVariant>
void ScenarioController<ModelVariant>::set_consumer_forcing(Consumer<ModelVariant>* consumer, const Forcing& forcing) {
    if (forcing < consumer->forcing()) {
        consumer->forcing(forcing);
    }
}


template<class ModelVariant>
std::string ScenarioController<ModelVariant>::time_units_str() const {
    if (scenario_node.has("baseyear")) {
        return std::string("days since ") + std::to_string(scenario_node["baseyear"].template as<unsigned int>()) + "-1-1";
    }
    return "days since 0-1-1";
}

INSTANTIATE_BASIC(Scenario);
}  // namespace acclimate
