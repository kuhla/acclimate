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

#include "output/DamageOutput.h"
#include "model/Model.h"
#include "model/Region.h"
#include "model/Sector.h"
#include "scenario/Scenario.h"
#include "variants/ModelVariants.h"

namespace acclimate {

template<class ModelVariant>
DamageOutput<ModelVariant>::DamageOutput(const settings::SettingsNode& settings_p,
                                         Model<ModelVariant>* model_p,
                                         Scenario<ModelVariant>* scenario_p,
                                         settings::SettingsNode output_node_p)
    : Output<ModelVariant>(settings_p, model_p, scenario_p, std::move(output_node_p)), damage(0.0) {
    out = nullptr;
}

template<class ModelVariant>
void DamageOutput<ModelVariant>::initialize() {
    if (!output_node.has("file")) {
        out = &std::cout;
    } else {
        std::string filename = output_node["file"].template as<std::string>();
        outfile.reset(new std::ofstream());
        outfile->open(filename.c_str(), std::ofstream::out);
        out = outfile.get();
    }
}

template<class ModelVariant>
void DamageOutput<ModelVariant>::internal_iterate_end() {
    for (const auto& sector : model->sectors_C) {
        for (const auto& firm : sector->firms_N) {
            damage += firm->total_loss().get_quantity();
        }
    }
}

template<class ModelVariant>
void DamageOutput<ModelVariant>::internal_end() {
    *out << damage << std::endl;
}

INSTANTIATE_BASIC(DamageOutput);
}  // namespace acclimate
