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

#include "scenario/DirectShock.h"
#include <random>
#include <utility>
#include "model/EconomicAgent.h"
#include "model/Region.h"
#include "model/Sector.h"
#include "run.h"
#include "variants/ModelVariants.h"

namespace acclimate {

template<class ModelVariant>
DirectShock<ModelVariant>::DirectShock(const settings::SettingsNode& settings_p, settings::SettingsNode scenario_node_p, Model<ModelVariant>* const model_p)
    : Scenario<ModelVariant>(settings_p, scenario_node_p, model_p) {}


template<class ModelVariant>
void DirectShock<ModelVariant>::apply_target(const settings::SettingsNode& node, const bool reset) {
    for (const auto& targets : node.as_sequence()) {
        for (const auto& target : targets.as_map()) {
            const std::string& type = target.first;
            const settings::SettingsNode& it = target.second;
            if (type == "firm") {
                if (it.has("sector")) {
                    if (it.has("region")) {
                        Firm<ModelVariant>* firm = model()->find_firm(it["sector"].template as<std::string>(), it["region"].template as<std::string>());
                        if (firm) {
                            if (it.has("remaining_capacity")) {
                                model()->run()->scenario_controller()->set_firm_forcing(firm, reset ? 1.0 : it["remaining_capacity"].as<Forcing>() / firm->capacity_manager->possible_overcapacity_ratio_beta);
                            } else if (it.has("forcing")) {
                                model()->run()->scenario_controller()->set_firm_forcing(firm, reset ? 1.0 : it["forcing"].as<Forcing>());
                            }
                        } else {
                            error("Firm " << it["sector"].template as<std::string>() << ":" << it["region"].template as<std::string>() << " not found");
                        }
                    } else {
                        Sector<ModelVariant>* sector = model()->find_sector(it["sector"].template as<std::string>());
                        if (sector) {
                            for (auto& p : sector->firms) {
                                if (it.has("remaining_capacity")) {
                                    model()->run()->scenario_controller()->set_firm_forcing(p, reset ? 1.0 : it["remaining_capacity"].as<Forcing>() / firm->capacity_manager->possible_overcapacity_ratio_beta);
                                } else if (it.has("forcing")) {
                                    model()->run()->scenario_controller()->set_firm_forcing(p, reset ? 1.0 : it["forcing"].as<Forcing>());
                                }
                            }
                        } else {
                            error("Sector " << it["sector"].template as<std::string>() << " not found");
                        }
                    }
                } else {
                    if (it.has("region")) {
                        Region<ModelVariant>* region = model()->find_region(it["region"].template as<std::string>());
                        if (region) {
                            for (auto& ea : region->economic_agents) {
                                if (ea->type == EconomicAgent<ModelVariant>::Type::FIRM) {
                                    if (it.has("remaining_capacity")) {
                                        model()->run()->scenario_controller()->set_firm_forcing(ea->as_firm(), reset ? 1.0 : it["remaining_capacity"].as<Forcing>() / firm->capacity_manager->possible_overcapacity_ratio_beta);
                                    } else if (it.has("forcing")) {
                                        model()->run()->scenario_controller()->set_firm_forcing(ea->as_firm(), reset ? 1.0 : it["forcing"].as<Forcing>());
                                    }
                                }
                            }
                        } else {
                            error("Region " << it["region"].template as<std::string>() << " not found");
                        }
                    } else {
                        for (auto& s : model()->sectors) {
                            for (auto& p : s->firms) {
                                if (it.has("remaining_capacity")) {
                                    model()->run()->scenario_controller()->set_firm_forcing(p, reset ? 1.0 : it["remaining_capacity"].as<Forcing>() / firm->capacity_manager->possible_overcapacity_ratio_beta);
                                } else if (it.has("forcing")) {
                                    model()->run()->scenario_controller()->set_firm_forcing(p, reset ? 1.0 : it["forcing"].as<Forcing>());
                                }
                            }
                        }
                    }
                }
            } else if (type == "consumer") {
                if (it.has("region")) {
                    Consumer<ModelVariant>* consumer = model()->find_consumer(it["region"].template as<std::string>());
                    if (consumer) {
                        model()->run()->scenario_controller->set_consumer_forcing(consumer, reset ? 1.0 : it["forcing"].as<Forcing>());
                    } else {
                        error("Consumer " << it["region"].template as<std::string>() << " not found");
                    }
                } else {
                    for (auto& r : model()->regions) {
                        for (auto& ea : r->economic_agents) {
                            if (ea->type == EconomicAgent<ModelVariant>::Type::CONSUMER) {
                                model()->run()->scenario_controller->set_consumer_forcing(ea->as_consumer(), reset ? 1.0 : it["forcing"].as<Forcing>());
                            }
                        }
                    }
                }
            } else if (type == "sea") {
                if (it.has("sea_route")) {
                    GeoLocation<ModelVariant>* location = model()->find_location(it["sea_route"].template as<std::string>());
                    if (location) {
                        if (it.has("passage")) {
                            model()->run()->scenario_controller()->set_location_forcing(location, reset ? 1.0 : it["sea_route"].as<Forcing>());
                        }
                    } else {
                        error("Sea route " << it["sea_route"].template as<std::string>() << " not found");
                    }
                }
            }
        }
    }
}



template<class ModelVariant>
Time DirectShock<ModelVariant>::start_time() {
    Time start_time;
    bool boolean_first_shock = true;
    for ( const auto& event : scenario_node["events"].as_sequence()) {
        const std::string& type = event["type"].template as<std::string>();
        if (type == "shock") {
            const Time from = event["from"].template as<Time>();
            if (boolean_first_shock || from < start_time ) {
                start_time = from;
                boolean_first_shock = false;
            }
        }
    }
    return start_time;
}

template<class ModelVariant>
bool DirectShock<ModelVariant>::iterate() {
    for (const auto& event : scenario_node["events"].as_sequence()) {
        const std::string& type = event["type"].template as<std::string>();
        if (type == "shock") {
            const Time from = event["from"].template as<Time>();
            const Time to = event["to"].template as<Time>();
            if (model()->time() >= from && model()->time() <= to) {
                apply_target(event["targets"], false);
            } else if (model()->time() == to + model()->delta_t()) {
                apply_target(event["targets"], true);
            }
        }
    }
    return true;
}


INSTANTIATE_BASIC(Scenario);
}  // namespace acclimate
