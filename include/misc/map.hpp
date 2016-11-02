#ifndef MISC_MAP
#define MISC_MAP

#include <map>

template <typename T>
typename T::mapped_type map_get(T const& map,
                                typename T::key_type const& key) {
    typename T::const_iterator iter(map.find(key));
    return iter != map.end() ? iter->second : typename T::mapped_type();
}

#endif // MISC_MAP
