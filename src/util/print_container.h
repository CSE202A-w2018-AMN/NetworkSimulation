#ifndef UTIL_PRINT_CONTAINER_H
#define UTIL_PRINT_CONTAINER_H

#include <iostream>

namespace print_container {

/**
 * An adapter that allows containers to be printed
 */
template <typename C>
class PrintContainer {
public:
    PrintContainer(const C& container) : container(container) {}
private:
    const C& container;
    friend std::ostream& operator << (std::ostream& stream, const PrintContainer& pc) {
        stream << '[';
        for (const auto& value : pc.container) {
            stream << ' ' << value;
        }
        stream << ']';
        return stream;
    }
};

/**
 * Returns an adapter that prints the elements in a container
 */
template <typename C>
PrintContainer<C> print(const C& container) {
    return PrintContainer<C>(container);
}

}




#endif
