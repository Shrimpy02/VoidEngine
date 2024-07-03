#pragma once
// Includes
#include <string>
#include <unordered_set>
#include <stdexcept>

/**
 * @class TagUnique
 * @brief Is a class managing all names in applications using a static sExistingTags.
 * If a name is already exists it throws an error in in addition to having helper functions for name/tag managing.
 */
struct TagUnique
{
private:
    // ----------- Variables ---------------

    static std::unordered_set<std::string> sExistingTags;
    std::string mValue;

public:
    // ----------- Functions ---------------

    // Constructs a tag object with a name. Throws an exception if a tag name already exists in the application.
    TagUnique(const std::string& _val)
    {
        if (sExistingTags.find(_val) != sExistingTags.end())
           throw std::runtime_error("Tag already exists: " + _val);
        
        mValue = _val;
        sExistingTags.insert(_val);
    }

    // Removes ability to move or copy object
    TagUnique(const TagUnique&) = delete;
    TagUnique& operator=(const TagUnique&) = delete;
    TagUnique(TagUnique&&) = default;
    TagUnique& operator=(TagUnique&&) = default;

    // deconstructs object and erases its tag from all existing tags
    ~TagUnique()
    {
        sExistingTags.erase(mValue);
    }

    // returns the name of the tag
    const std::string& GetValue() const
    {
        return mValue;
    }

    // returns true if tag exists in sExistingTags
    static bool TagExists(const std::string& tag)
    {
        return sExistingTags.find(tag) != sExistingTags.end();
    }

    // Clears all existing tags from memory
    static void ClearCache()
    {
        sExistingTags.clear();
        
    }
};

inline std::unordered_set<std::string> TagUnique::sExistingTags{};