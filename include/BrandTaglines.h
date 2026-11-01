#pragma once
// Brand banner taglines. The text comes from the 'brands' table in PostgreSQL, so adding or
// renaming a partner brand never needs a code change. A brand with no row in that table
// simply gets a generic line.
#include <string>
#include <utility>
#include <vector>
#include "Utils.h"       // toLower

class BrandTaglines {
    std::vector<std::pair<std::string, std::string> > rows_;   // (brand in lower case, tagline)

public:
    void add(const std::string& brand, const std::string& tagline) {
        rows_.push_back(std::make_pair(toLower(brand), tagline));
    }
    std::string lookup(const std::string& brand) const {
        std::string key = toLower(brand);
        for (size_t i = 0; i < rows_.size(); i++)
            if (rows_[i].first == key) return rows_[i].second;
        return "Official partner brand.";
    }
    size_t size() const { return rows_.size(); }
};
