#pragma once

#include <vector>

// struct UpdateOperation
// {
// UpdateOperation ();
// bool is_null () const; is operation no-op (should also be in case if it's default constructed)
// void apply (ValueType prev_range_value, ptrdiff_t range_length) cont -> new_range_value
// way to stack updates (may be something totally different from add):
// friend UpdateOperation operator+ (const UpdateOperation &, const UpdateOperation &) -> new_update_value
// }

// QueryOperation = function (ValueType, ValueType) -> ValueType

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
class segment_tree
{
public:
    segment_tree(const std::vector<ValueType>& initial_data)
    {
        init(initial_data);
    }

    void update(ptrdiff_t from, ptrdiff_t to, UpdateOperation op);
    ValueType query(ptrdiff_t from, ptrdiff_t to);

private:
    ValueType query_impl(ptrdiff_t req_from, ptrdiff_t req_to, ptrdiff_t index, ptrdiff_t from, ptrdiff_t to);
    void update_lazy(ptrdiff_t index, ptrdiff_t from, ptrdiff_t to);
    void update_impl(ptrdiff_t upd_from, ptrdiff_t upd_to, UpdateOperation op, ptrdiff_t index, ptrdiff_t from,
                     ptrdiff_t to);
    int construct(const std::vector<ValueType>& initial_data, ptrdiff_t from, ptrdiff_t to, ptrdiff_t index);
    void init(const std::vector<ValueType>& initial_data);

private:
    ptrdiff_t m_n;
    std::vector<ValueType> m_tree;
    std::vector<UpdateOperation> m_lazy;
    QueryOperation m_op;
};

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
ValueType segment_tree<ValueType, QueryOperation, UpdateOperation>::query_impl(ptrdiff_t req_from, ptrdiff_t req_to,
                                                                               ptrdiff_t index, ptrdiff_t from,
                                                                               ptrdiff_t to)
{
    update_lazy(index, from, to);

    if (from > to || req_from > to || req_to < from)
        return ValueType{};

    if (req_from <= from && req_to >= to)
        return m_tree[index];

    ptrdiff_t mid = from + (to - from) / 2;
    ValueType ret{};
    ret = m_op(ret, query_impl(req_from, req_to, 2 * index + 1, from, mid));
    ret = m_op(ret, query_impl(req_from, req_to, 2 * index + 2, mid + 1, to));
    return ret;
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
ValueType segment_tree<ValueType, QueryOperation, UpdateOperation>::query(ptrdiff_t from, ptrdiff_t to)
{
    return query_impl(from, to, 0, 0, m_n - 1);
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
void segment_tree<ValueType, QueryOperation, UpdateOperation>::update_lazy(ptrdiff_t index, ptrdiff_t from,
                                                                           ptrdiff_t to)
{
    if (!m_lazy[index].is_null())
    {
        m_tree[index] = m_lazy[index].apply(m_tree[index], to - from + 1);
        if (from != to)
        {
            m_lazy[index * 2 + 1] = m_lazy[index * 2 + 1] + m_lazy[index];
            m_lazy[index * 2 + 2] = m_lazy[index * 2 + 2] + m_lazy[index];
        }
        m_lazy[index] = {};
    }
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
void segment_tree<ValueType, QueryOperation, UpdateOperation>::update_impl(ptrdiff_t upd_from, ptrdiff_t upd_to,
                                                                           UpdateOperation op, ptrdiff_t index,
                                                                           ptrdiff_t from, ptrdiff_t to)
{
    update_lazy(index, from, to);

    if (from > to || upd_from > to || upd_to < from)
        return;

    if (upd_from <= from && to <= upd_to)
    {
        m_tree[index] = op.apply(m_tree[index], to - from + 1);
        if (from != to)
        {
            m_lazy[index * 2 + 1] = m_lazy[index * 2 + 1] + op;
            m_lazy[index * 2 + 2] = m_lazy[index * 2 + 2] + op;
        }
        return;
    }

    ptrdiff_t mid = from + (to - from) / 2;
    update_impl(upd_from, upd_to, op, 2 * index + 1, from, mid);
    update_impl(upd_from, upd_to, op, 2 * index + 2, mid + 1, to);

    m_tree[index] = m_op(m_tree[2 * index + 1], m_tree[2 * index + 2]);
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
void segment_tree<ValueType, QueryOperation, UpdateOperation>::update(ptrdiff_t from, ptrdiff_t to, UpdateOperation op)
{
    return update_impl(from, to, op, 0, 0, m_n - 1);
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
int segment_tree<ValueType, QueryOperation, UpdateOperation>::construct(const std::vector<ValueType>& initial_data,
                                                                        ptrdiff_t from, ptrdiff_t to, ptrdiff_t index)
{
    if (from == to)
        return (m_tree[index] = initial_data[from]);

    ptrdiff_t mid = from + (to - from) / 2;
    m_tree[index] = m_op(m_tree[index], construct(initial_data, from, mid, index * 2 + 1));
    m_tree[index] = m_op(m_tree[index], construct(initial_data, mid + 1, to, index * 2 + 2));
    return m_tree[index];
}

template <typename ValueType, typename QueryOperation, typename UpdateOperation>
void segment_tree<ValueType, QueryOperation, UpdateOperation>::init(const std::vector<ValueType>& initial_data)
{
    m_n = initial_data.size();
    m_tree.resize((1ULL << (1 + static_cast<int>(std::ceil(std::log2(initial_data.size()))))) - 1);
    m_lazy.resize(m_tree.size());
    construct(initial_data, 0, m_n - 1, 0);
}
