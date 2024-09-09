#pragma once

// Includes
#include <filesystem>

// Additional Includes

// Forward Declarations

// Namespace declaration
namespace fs = std::filesystem;

// Inline function returns the relative path from in base path and in absolute path as std::string
inline std::string ToRelativePath(const std::string& basePath, const std::string& absolutePath) {
    fs::path base(basePath);
    fs::path absolute(absolutePath);
    fs::path relative = fs::relative(absolute, base);
    return relative.string();
}

// Inline function returns the name from in filepath as an std::string
inline std::string GetFileNameFromPath(const std::string& filePath)
{
    size_t pos = filePath.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        return filePath.substr(pos + 1);
    }
    return filePath;
}

// Inline function returns the directory path from in filepath as an std::string
inline std::string GetDirectoryPath(const std::string& filePath)
{
    size_t pos = filePath.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        return filePath.substr(0, pos + 1);
    }
    return "";
}

// Inline function returns the file extension from in filepath as an std::string
inline std::string GetFileExtension(const std::string& filePath) {
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        return filePath.substr(pos);
    }
    return "";
}

// Inline function returns the in file path without the ending file extension
inline std::string RemoveFileExtension(const std::string& filePath)
{
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        return filePath.substr(0, pos);
    }
    return filePath;
}

// Inline function returns true if in file path contains "_AABBCollision_" and assign that string value to the in prefix pointer
inline bool HasCollisionAABBPrefix(const std::string& filePath, std::string& outPrefix)
{
    if (filePath[0] != '_')
        return false;

    size_t pos = std::string(filePath.begin() + 1, filePath.end()).find_first_of('_');
    if (pos == std::string::npos)
        return false;

    if (std::string(filePath.begin(), filePath.begin() + pos + 2) != "_AABBCollision_")
        return false;

    outPrefix = std::string(filePath.begin() + pos + 2, filePath.end());

    return true;
}


// Inline function returns true if in file path contains "_BoundingSphereCollision_" and assign that string value to the in prefix pointer
inline bool HasCollisionBoundignSphererePrefix(const std::string& filePath, std::string& outPrefix)
{
    if (filePath[0] != '_')
        return false;

    size_t pos = std::string(filePath.begin() + 1, filePath.end()).find_first_of('_');
    if (pos == std::string::npos)
        return false;

    if (std::string(filePath.begin(), filePath.begin() + pos + 2) != "_BoundingSphereCollision_")
        return false;

    outPrefix = std::string(filePath.begin() + pos + 2, filePath.end());

    return true;
}

// Inline function returns true if in file path contains "_Light_" and assign that string value to the in prefix pointer
inline bool HasLightPrefix(const std::string& filePath, std::string& outPrefix)
{
    if (filePath[0] != '_')
        return false;

    size_t pos = std::string(filePath.begin() + 1, filePath.end()).find_first_of('_');
    if (pos == std::string::npos)
        return false;

    if (std::string(filePath.begin(), filePath.begin() + pos + 2) != "_Light_")
        return false;

    outPrefix = std::string(filePath.begin() + pos + 2, filePath.end());

    return true;
}