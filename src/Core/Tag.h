#pragma once
// Includes
#include <string>
#include <unordered_set>
#include <sstream>
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
        std::string tag = IncrementTag(_val);
        
        mValue = tag;
        sExistingTags.insert(tag);
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

    void RenameTag(std::string _newName)
    {
        mValue = _newName;
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

    static std::string IncrementTag(std::string _tag)
    {
        // Check if the base tag already exists
        if (!TagExists(_tag))
        {
            return _tag;
        }

        // If the base tag exists, find a unique tag by appending a number
        int suffix = 1;
        std::string newTag;
        do
        {
            std::ostringstream oss;
            oss << _tag << "_" << suffix;
            newTag = oss.str();
            ++suffix;
        } while (TagExists(newTag));

        return newTag;
    }

    // Clears all existing tags from memory
    static void ClearCache()
    {
        sExistingTags.clear();
        
    }

    static void RemoveTagFromCache(std::string _tagToRemove)
    {
        if (sExistingTags.find(_tagToRemove) != sExistingTags.end())
            sExistingTags.erase(_tagToRemove);
    }

    void Rename(std::string _newTag)
    {
        std::string tag = IncrementTag(_newTag);

        mValue = tag;
        sExistingTags.insert(tag);
    }

};

inline std::unordered_set<std::string> TagUnique::sExistingTags{};