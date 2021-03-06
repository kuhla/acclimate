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

#ifndef ACCLIMATE_GOVERNMENT_H
#define ACCLIMATE_GOVERNMENT_H

#include <unordered_map>
#include "acclimate.h"

namespace acclimate {

template<class ModelVariant>
class Region;
template<class ModelVariant>
class Firm;

template<class ModelVariant>
class Government {
  public:
    Region<ModelVariant>* const region;

  protected:
    Value budget_;

  private:
    std::unordered_map<Firm<ModelVariant>*, Ratio> taxed_firms;

  public:
    inline const Value& budget() const { return budget_; };

  protected:
    void collect_tax();
    void redistribute_tax();
    void impose_tax();

  public:
    explicit Government(Region<ModelVariant>* region_p);
    void iterate_consumption_and_production();
    void iterate_expectation();
    void iterate_purchase();
    void iterate_investment();
    void define_tax(const std::string& sector, const Ratio& tax_ratio_p);
    inline std::string id() const { return "GOVM:" + region->id(); }
};
}  // namespace acclimate

#endif
