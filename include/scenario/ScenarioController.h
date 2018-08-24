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

#ifndef ACCLIMATE_SCENARIOCONTROLLER_H
#define ACCLIMATE_SCENARIOCONTROLLER_H

#include <string>
#include "model/Consumer.h"
#include "model/Firm.h"
#include "model/Model.h"
#include "settingsnode.h"
#include "types.h"

namespace acclimate {

template<class ModelVariant>
class ScenarioController {
  protected:
    void set_firm_forcing(Firm<ModelVariant>* firm, const Forcing& forcing);
    void set_consumer_forcing(Consumer<ModelVariant>* consumer, const Forcing& forcing);
    void set_location_forcing(GeoLocation<ModelVariant>* location, const Forcing& forcing);
    std::vector<Scenario<ModelVariant>*> scenarios;

  public:
    ScenarioController();
    virtual ~ScenarioController() {}
};
}  // namespace acclimate

#endif
