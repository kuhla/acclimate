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

#include "scenario/EventSeriesScenario.h"

#include <memory>
#include "model/Model.h"
#include "model/Sector.h"
#include "run.h"
#include "variants/ModelVariants.h"

namespace acclimate {

template<class ModelVariant>
EventSeriesScenario<ModelVariant>::EventSeriesScenario(const settings::SettingsNode& settings_p, settings::SettingsNode scenario_node_p, Model<ModelVariant>* const model_p)
    : ExternalScenario<ModelVariant>(settings_p, scenario_node_p, model_p) {}

template<class ModelVariant>
ExternalForcing* EventSeriesScenario<ModelVariant>::read_forcing_file(const std::string& filename, const std::string& variable_name) {
    return new EventForcing(filename, variable_name, model());
}

template<class ModelVariant>
void EventSeriesScenario<ModelVariant>::read_forcings() {
    {
        auto forcing_l = static_cast<EventForcing*>(forcing.get());
        for (std::size_t i = 0; i < forcing_l->firms.size(); ++i) {
            if (forcing_l->firms[i]) {
                if (std::isnan(forcing_l->forcings[i])) {
                    forcing_l->firms[i]->forcing(1.0);
                } else {
                    forcing_l->firms[i]->forcing(forcing_l->forcings[i]);
                }
            }
        }
    }
    {
        std::vector<Sector<ModelVariant>*> sectors;
        auto forcing_l = static_cast<EventForcing*>(forcing.get());
        for (std::size_t i = 0; i < forcing_l->locations.size(); ++i) {
            if (forcing_l->locations[i]) {
                if(forcing_l->forcings[i] < 0.0) {
                    forcing_l->locations[i]->set_forcing_nu( -1., sectors);
                } else {
                    forcing_l->locations[i]->set_forcing_nu( forcing_l->forcings[i], sectors);
                }
            }
        }
    }
}

template<class ModelVariant>
void EventSeriesScenario<ModelVariant>::EventForcing::read_data() {
    if (firms.size() != 0) {
        variable.getVar({time_index, 0, 0}, {1, sectors_count, regions_count}, &forcings[0]);
    } else if (locations.size() != 0) {
        variable.getVar({time_index, 0}, {1, sea_objects_count}, &forcings[0]);
    }
}

template<class ModelVariant>
EventSeriesScenario<ModelVariant>::EventForcing::EventForcing(const std::string& filename, const std::string& variable_name, const Model<ModelVariant>* model)
    : ExternalForcing(filename, variable_name) {
        
    auto dims = file->getDims();
    auto region_dim = dims.find("region");
    auto sector_dim = dims.find("sector");
    auto sea_dim = dims.find("sea_route");
    if ( region_dim != std::end(dims) && sector_dim != std::end(dims)) {
        
        auto region_dim_length = region_dim->second.getSize();
        auto sector_dim_length = sector_dim->second.getSize();
        std::vector<const char*> regions(region_dim_length);
        file->getVar("region").getVar(&regions[0]);
        std::vector<const char*> sectors(sector_dim_length);
        file->getVar("sector").getVar(&sectors[0]);
        regions_count = regions.size();
        sectors_count = sectors.size();
        firms.reserve(regions_count * sectors_count);
        forcings.reserve(regions_count * sectors_count);
        for (const auto& sector_name : sectors) {
            Sector<ModelVariant>* sector = model->find_sector(sector_name);
            if (!sector) {
                error_("sector '" + std::string(sector_name) + "' not found");
            }
            for (const auto& region_name : regions) {
                Firm<ModelVariant>* firm = model->find_firm(sector, region_name);
                if (!firm) {
                    warning_("firm '" + std::string(sector_name) + ":" + std::string(region_name) + "' not found");
                }
                firms.push_back(firm);
                forcings.push_back(1.0);
            }
        }
    } else if (sea_dim != std::end(dims)) {
        auto sea_dim_length = sea_dim->second.getSize();
        std::vector<const char*> sea_objects(sea_dim_length);
        file->getVar("sea_route").getVar(&sea_objects[0]);
        sea_objects_count = sea_objects.size();
        locations.reserve(sea_objects_count);
        forcings.reserve(sea_objects_count);
        for (const auto& sea_object : sea_objects) {
            GeoLocation<ModelVariant>* location = model->find_location(sea_object);
            if (!location) {
                warning_("sea object '" + std::string(sea_object) + "' not found");
            }
            locations.push_back(location);
            forcings.push_back(-1.);
        }
    }

}

INSTANTIATE_BASIC(EventSeriesScenario);
}  // namespace acclimate
