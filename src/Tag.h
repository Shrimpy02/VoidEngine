#pragma once
#include <string>
#include <unordered_set>
#include <stdexcept>

struct TagUnique
{
private:
    // ----------- Variables ---------------
    static std::unordered_set<std::string> sExistingTags;
    std::string mValue;

public:
    // ----------- Functions ---------------
    TagUnique(const std::string& val)
    {
        if (sExistingTags.find(val) != sExistingTags.end())
        {
           throw std::runtime_error("Tag already exists: " + val);
        }
        mValue = val;
        sExistingTags.insert(val);
    }

    TagUnique(const TagUnique&) = delete;
    TagUnique& operator=(const TagUnique&) = delete;

    TagUnique(TagUnique&&) = default;
    TagUnique& operator=(TagUnique&&) = default;

    ~TagUnique()
    {
        sExistingTags.erase(mValue);
    }

    const std::string& GetValue() const
    {
        return mValue;
    }

    static bool TagExists(const std::string& tag)
    {
        return sExistingTags.find(tag) != sExistingTags.end();
    }

    static void ClearCache()
    {
        sExistingTags.clear();
    }

    // Adders ---------------

    // Setters ---------------


    // Getters ---------------
};

inline std::unordered_set<std::string> TagUnique::sExistingTags{};