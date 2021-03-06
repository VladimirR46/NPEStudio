/* Copyright 2015-2020 The MathWorks, Inc. */

#ifndef coder_tgtsvc_detail_fifo_hpp
#define coder_tgtsvc_detail_fifo_hpp

namespace coder { namespace tgtsvc { namespace detail {

template <typename valueType>
struct circular_iterator
{
   
    typedef valueType value_type;
    typedef value_type * pointer;
    typedef value_type & reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    explicit circular_iterator(valueType *it, valueType *begin, valueType *end) : it_(it), begin_(begin), end_(end) {}

    bool operator==(const circular_iterator<valueType> &rhs) const { return rhs.it_ == it_; }
    bool operator!=(const circular_iterator<valueType> &rhs) const { return !(rhs == *this); }

    circular_iterator<valueType> &operator+=(int dist) { advance(dist); return *this; }
    circular_iterator<valueType> &operator-=(int dist) { advance(-dist); return *this; }
    circular_iterator<valueType> &operator++() { advance(); return *this; }
    circular_iterator<valueType> &operator--() { retreat(); return *this; }
    circular_iterator<valueType>  operator++(int) { coder::tgtsvc::detail::circular_iterator<valueType> temp(*this); advance(); return temp; }
    circular_iterator<valueType>  operator--(int) { coder::tgtsvc::detail::circular_iterator<valueType> temp(*this); retreat(); return temp; }
    circular_iterator<valueType> operator+(ptrdiff_t dist) const { coder::tgtsvc::detail::circular_iterator<valueType> temp(*this); temp.advance(dist); return temp; }
    circular_iterator<valueType> operator-(ptrdiff_t dist) const { coder::tgtsvc::detail::circular_iterator<valueType> temp(*this); temp.advance(-dist); return temp; }

    ptrdiff_t operator-(const circular_iterator<valueType> &rhs) const {
        ptrdiff_t r = it_ - rhs.it_;
        if (r < 0) r += (end_ - begin_);
        return r;
    }

    valueType &operator*() { return *it_; }
    const valueType &operator*() const { return *it_; }

    valueType &operator[](size_t idx) {
        valueType *r = it_ + idx;
        ptrdiff_t wrap = end_ - begin_;
        while (r >= end_) { r -= wrap; }
        return *r;
    }

private:
    valueType *it_;
    valueType *begin_;
    valueType *end_;

    void advance() {
        valueType *out = ++it_;
        if (out == end_) out = begin_;
        it_ = out;
    }

    void advance(ptrdiff_t dist)
    {
        valueType *out = it_ + dist;
        ptrdiff_t wrap = end_ - begin_;
        while (out >= end_) { out -= wrap; }
        while (out < begin_) { out += wrap; }
        it_ = out;
    }

    void retreat() {
        valueType *out = --it_;
        if (out < begin_) out = end_ - 1;
        it_ = out;
    }
};

template<typename T, size_t N>
class fifo
{
public:

    typedef T valueType;

    struct carray {
        T *addr_;
        size_t size_;
    };

    fifo() : contents_(buff_), space_(buff_) {}

    bool empty() const { return space_ == contents_; }
    bool full() const { return contents_size() == N - 1; }
    void clear() { contents_ = buff_; space_ = buff_; }

    T front() const { return *contents_; }
    void pop() { contents_remove(1); }
    void push(const T &val) {
        *space_ = val;
        contents_add(1);
    }

    T &operator[](ptrdiff_t idx) { return contents_at(idx); }

    size_t contents_size() const {
        return contents_wrapped() ?  N + space_ - contents_ : space_ - contents_;
    }

    T &contents_at(size_t idx) {
        T *p = increment(const_cast<T*>(contents_), idx);
        return *p;
    }

    carray contents_carray() {
        carray r;
        r.addr_ = const_cast<T*>(contents_);
        r.size_ = contents_wrapped() ? buff_end() - contents_ : space_ - contents_;
        return r;
    }

    void contents_remove(size_t count) {
        contents_ = increment(const_cast<T*>(contents_), count);
    }

    void contents_add(size_t count) {
        space_ = increment(const_cast<T*>(space_), count);
    }

    size_t space_size() const { return N - contents_size() - 1; }

    T &space_at(size_t idx) {
        T *p = increment(const_cast<T*>(space_), idx);
        return *p;
    }

    carray space_carray() {
        carray r;
        r.addr_ = const_cast<T*>(space_);
        if (contents_wrapped()) {
            r.size_ = contents_ - space_ - 1;
        }
        else {
            r.size_ = buff_end() - space_;

            if (contents_ == buff_) --r.size_;
        }
        return r;
    }

    circular_iterator<valueType> contents_begin() { return circular_iterator<valueType>(const_cast<T*>(contents_), buff_, buff_ + N);	}
    circular_iterator<valueType> contents_end()   { return circular_iterator<valueType>(const_cast<T*>(space_), buff_, buff_ + N); }

    circular_iterator<valueType> space_begin() { return circular_iterator<valueType>(const_cast<T*>(space_), buff_, buff_ + N); }
    circular_iterator<valueType> space_end()   { return --contents_begin(); }

private:
    volatile T *contents_;
    volatile T *space_;
    T buff_[N];

    T *increment(T *p, size_t i=1) {
        p += i;
        p = p >= buff_end() ? p - N : p;
        return p;
    }

    T *buff_end() { return buff_ + N; }
    bool contents_wrapped() const { return space_ < contents_; }

};

}}}

#endif
