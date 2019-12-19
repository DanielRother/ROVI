#ifndef __ENUMUTILS_H__
#define __ENUMUTILS_H__

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

#endif /* __ENUMUTILS_H__ */
