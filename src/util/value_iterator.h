#ifndef UTIL_VALUE_ITERATOR_H
#define UTIL_VALUE_ITERATOR_H

namespace util {

/**
 * Adapts an iterator over pairs of values, yielding the second of each
 * pair
 */
template <typename I>
class ValueIterator {
private:
    /** The underlying iterator that yields pairs */
    I _iter;
public:
    typedef decltype(_iter->second) item;

    ValueIterator(I iter) :
        _iter(iter)
    {
    }

    I inner() const {
        return _iter;
    }

    ValueIterator& operator ++ () {
        _iter++;
        return *this;
    }
    ValueIterator& operator -- () {
        _iter--;
        return *this;
    }

    item& operator * () {
        return _iter->second;
    }
    item* operator -> () {
        return &_iter->second;
    }

    bool operator == (const ValueIterator& other) const {
        return this->_iter == other._iter;
    }
    bool operator != (const ValueIterator& other) const {
        return this->_iter != other._iter;
    }
};

}
#endif
