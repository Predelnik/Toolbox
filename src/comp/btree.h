#pragma once
#include <memory>
#include <map>
#include <cassert>
#include <array>
#include <iterator>

#ifndef _MSC_VER
#define TYPENAME_IF_NOT_MSVC typename // due to bug in msvc with parsing variadic template templates
#else
#define TYPENAME_IF_NOT_MSVC
#endif // _MSC_VER

namespace btree
{
  enum class color_t : char
  {
    red,
    black,
  };

  enum direction_t
  {
    none = -1,
    left = 0,
    right = 1,
  };

  inline direction_t other_direction(direction_t d)
  {
    return d == left ? right : left;
  }

  template <typename KeyType, template <typename> class... Plugins>
  struct rb_tree_container;

  template <typename KeyType, typename NodeType, template <typename> class... Plugins>
  struct btree_container;

  template <typename NodeType>
  class indexation_plugin
  {
  public:
    class node_t
    {
    public:
      std::size_t node_count() const { return m_node_count; }

    private:
      std::size_t m_node_count = 1;

    public:

      void on_new_child()
      {
        auto current = static_cast<NodeType *>(this)->parent();
        while (current)
        {
          current->m_node_count += m_node_count;
          current = current->parent();
        }
      }

      void before_takeout()
      {
        auto current = static_cast<NodeType *>(this)->parent();
        while (current)
        {
          current->m_node_count -= m_node_count;
          current = current->parent();
        }
      }
    };

    template <typename TreeType>
    class tree_t
    {
    public:
      template <typename ArgType>
      std::size_t index(const ArgType& key)
      {
        auto current = static_cast<TreeType*>(this)->root();
        std::size_t index_so_far = 0;
        while (current)
        {
          if (key < current->key())
            current = current->child(left);
          else if (current->key() < key)
          {
            index_so_far += (current->child(left) ? current->child(left)->node_count() : 0) + 1;
            current = current->child(right);
          }
          else
          {
            index_so_far += (current->child(left) ? current->child(left)->node_count() : 0);
            break;
          }
        }
        if (current)
          return index_so_far;

        return static_cast<std::size_t>(-1);
      }
    };
  };

  template <typename KeyType, typename NodeType, template <typename> class... Plugins>
  struct btree_container
  {
    class tree_t;
    class node_t;
    class iterator_t;
    class base_node_t;
    using node_storage_t = std::unique_ptr<base_node_t, void (*)(base_node_t *)>;
    static void delete_node(base_node_t *node) { delete static_cast<NodeType *> (node); }
    template <typename ArgType>
    static node_storage_t make_node (ArgType &&key) { return node_storage_t (new NodeType (std::forward<ArgType> (key)), &delete_node); } 

  public:
    class base_node_t
    {
    protected:
      base_node_t (bool is_sentinel_arg) : is_sentinel (is_sentinel_arg) {}
      bool is_sentinel : 1;
      friend class node_t;
      friend class iterator_t;
    };

    class sentinel_t : public base_node_t
    {
    public:
      sentinel_t(tree_t* tree_ptr) : base_node_t (true), m_tree_ptr(tree_ptr) {}
    private:
      tree_t* m_tree_ptr;
      friend class iterator_t;
    };

    class node_t : public base_node_t, public Plugins<NodeType>::node_t...
    {
    public:
      template <typename ArgType>
      explicit node_t(ArgType&& key) : base_node_t (false),
        m_children {node_storage_t {nullptr, &delete_node}, node_storage_t {nullptr, &delete_node}},
        m_key(std::forward<ArgType>(key))
      {
      }
      ~node_t (){}

      const auto& key() const { return m_key; }

      const NodeType* child(direction_t direction) const
      {
        return m_children[direction] && !m_children[direction]->is_sentinel ? static_cast<const NodeType*>(m_children[direction].get()) : nullptr;
      }

      NodeType* child(direction_t direction)
      {
        return m_children[direction] && !m_children[direction]->is_sentinel ? static_cast<NodeType*>(m_children[direction].get()) : nullptr;
      }

      const NodeType* parent() const { return m_parent; }
      NodeType* parent() { return m_parent; }

      NodeType* furthest_node(direction_t direction)
      {
        auto cur_node = this;
        while (auto next_node = cur_node->child(direction))
          cur_node = next_node;
        return static_cast<NodeType *>(cur_node);
      }

      NodeType* next_node(direction_t direction)
      {
        if (auto c = child(direction))
        {
          return c->furthest_node(other_direction(direction));
        }

        auto cur_node = this;
        while (cur_node->parent() && cur_node->direction_from_parent() == direction)
          cur_node = cur_node->parent();
        return cur_node->parent();
      }

      NodeType* uncle()
      {
        assert (m_parent);
        return m_parent->sibling();
      }

      NodeType* sibling()
      {
        return m_parent ? m_parent->child(other_direction(direction_from_parent())) : nullptr;
      }

      int child_count() const
      {
        int cnt = 0;
        for (auto direction : {left, right})
          cnt += (child(direction) != nullptr);
        return cnt;
      }

      NodeType* single_child()
      {
        assert (child_count() <= 1);
        for (auto child_direction : {left, right})
        {
          if (auto c = child(child_direction))
            return c;
        }
        return nullptr;
      }

      bool is_root()
      {
        return m_parent == nullptr;
      }

      direction_t direction_from_parent() const
      {
        if (!m_parent)
          return none;
        for (auto b : {left, right})
          if (m_parent->m_children[b].get() == this)
            return b;
        assert (false);
        return left;
      }

      node_storage_t take_out()
      {
        if (this->is_sentinel)
          return {nullptr, &delete_node};
        std::initializer_list<int>{(static_cast<TYPENAME_IF_NOT_MSVC Plugins<NodeType>::node_t*>(this)->before_takeout() , 0)...};
        auto parent = this->m_parent;
        auto direction = direction_from_parent();
        this->m_parent = nullptr;
        auto raw_ptr = parent->m_children[direction].release ();
        return node_storage_t (static_cast<NodeType *>(raw_ptr), &delete_node);
      }

      NodeType* append_child(direction_t dir, node_storage_t node)
      {
        assert (m_children[dir] == nullptr || m_children[dir]->is_sentinel);
        m_children[dir] = std::move(node);
        if (!m_children[dir]->is_sentinel)
        {
          static_cast<NodeType *>(m_children[dir].get())->m_parent = static_cast<NodeType *>(this);
          std::initializer_list<int>{(static_cast<TYPENAME_IF_NOT_MSVC Plugins<NodeType>::node_t*>(this->child(dir))->on_new_child() , 0)...};
        }

        return this->child(dir);
      }

    private:
      std::array<node_storage_t, 2> m_children;
      NodeType* m_parent = nullptr;
      KeyType m_key;

      friend class tree_t;
      friend class iterator_t;
    };

    class iterator_t
    {
      using node_type = NodeType;
      using self = iterator_t;
    public:
      using iterator_category = std::bidirectional_iterator_tag;
      using reference = const KeyType &;
      using pointer = const KeyType *;
      using value_type = const KeyType;
      using difference_type = std::size_t;
    public:
      iterator_t(base_node_t* node) : m_node_ptr(node)
      {
      }

      self& operator++()
      {
        auto next_node = get_node()->next_node(right);
        if (next_node == nullptr)
        {
          m_node_ptr = get_node()->m_children[right].get ();
          assert (m_node_ptr->is_sentinel);
        }
        else
          m_node_ptr = next_node;
        return *this;
      }

      self& operator--()
      {
        if (!m_node_ptr->is_sentinel)
          m_node_ptr = get_node()->next_node(left);
        else
          m_node_ptr = get_tree()->furthest_node(right);

        return *this;
      }

      reference operator*()
      {
        return get_node()->key();
      }

      bool operator==(const self& other)
      {
        return m_node_ptr == other.m_node_ptr;
      }

      bool operator!=(const self& other)
      {
        return !(*this == other);
      }

    private:
      node_t* get_node() const
      {
        assert (!m_node_ptr->is_sentinel);
        return static_cast<node_t *>(m_node_ptr);
      }

      tree_t* get_tree() const
      {
        assert (m_node_ptr->is_sentinel);
        return static_cast<sentinel_t *>(m_node_ptr)->m_tree_ptr;
      }

    private:
      base_node_t* m_node_ptr;
    };

    class tree_t : public Plugins<NodeType>::template tree_t<tree_t>...
    {
      using self = tree_t;
    public:
      using iterator = iterator_t;

      tree_t () : m_sentinel (this), m_size (0), m_root (nullptr, &delete_node)
      {
        
      }

      iterator_t begin()
      {
        return {m_root ? furthest_node(left) : nullptr};
      }

      iterator_t end()
      {
        return {&m_sentinel};
      }


      void clear()
      {
        m_root.reset();
        m_furthest_node.fill (nullptr);
        m_size = 0;
      }

      node_storage_t take_out(NodeType* node)
      {
        if (node->parent())
          return node->take_out();
        else
          return std::move(m_root);
      }

      void replace_with_child(NodeType* node)
      {
        auto cnt = node->child_count();
        auto direction = node->direction_from_parent();
        auto parent = node->parent();
        auto taken_out = take_out(node);
        auto update_furthest_nodes =
        [&]()
        {
          for (auto furthest_node_dir : {left, right})
            if (node == m_furthest_node[furthest_node_dir])
              {
                if (parent)
                  {
                    if (auto child = parent->child (furthest_node_dir))
                      m_furthest_node[furthest_node_dir] = child;
                    else
                      m_furthest_node[furthest_node_dir] = parent;
                  }
                else
                  m_furthest_node[furthest_node_dir] = static_cast<NodeType *> (m_root.get ());
                if (furthest_node_dir == right && m_furthest_node[right])
                  m_furthest_node[right]->append_child (right, make_sentinel());
              }
        };

        if (cnt == 0)
        {
          update_furthest_nodes ();
          return;
        }
        if (parent)
          parent->append_child(direction, take_out(node->single_child()));
        else
          m_root = node->single_child()->take_out();
        update_furthest_nodes ();
      }

      void rotate(NodeType* node, direction_t dir)
      {
        auto prev_parent = node->parent();
        direction_t prev_direction;
        if (prev_parent)
          prev_direction = node->direction_from_parent();
        auto hanging_node = take_out(node);
        auto heritor = take_out(node->child(other_direction(dir)));
        auto heritor_ptr = static_cast<NodeType *> (heritor.get ());
        if (heritor_ptr->child(dir))
          node->append_child(other_direction(dir), take_out(heritor_ptr->child(dir)));

        heritor_ptr->append_child(dir, std::move(hanging_node));
        if (prev_parent)
          prev_parent->append_child(prev_direction, std::move(heritor));
        else
          m_root = std::move(heritor);
      }

      template <typename ArgType>
      std::size_t erase(const ArgType& key)
      {
        auto current = preerase(key);
        if (!current)
          return 0;

        replace_with_child(current);
        return 1;
      }

      std::size_t size() const { return m_size; }

      const NodeType* root() const { return static_cast<NodeType *> (m_root.get()); }

    protected:
      NodeType* furthest_node(direction_t direction)
      {
        return m_furthest_node[direction];
      }

      template <typename ArgType>
      NodeType* preinsert(ArgType&& key)
      {
        auto create_node = [](auto&& key)
        {
          auto node = make_node (std::forward<ArgType>(key));
          return node;
        };

        auto current = static_cast<NodeType *> (m_root.get());
        if (!current)
        {
          m_root = create_node(std::forward<ArgType>(key));
          current = static_cast<NodeType *> (m_root.get());
        }
        else
        {
          while (true)
          {
            auto check_child = [&](direction_t direction)
            {
              if (!current->child(direction))
              {
                current = current->append_child(direction, create_node(std::forward<ArgType>(key)));
                return true;
              }

              current = current->child(direction);
              return false;
            };

            if (key < current->key())
            {
              if (check_child(left))
                break;
            }
            else
            {
              if (check_child(right))
                break;
            }
          }
        }
        for (auto direction : {left, right})
          if (m_furthest_node[direction] == nullptr ||
            current == m_furthest_node[direction]->child(direction))
          {
            m_furthest_node[direction] = current;
            if (direction == right)
              m_furthest_node[direction]->append_child(right, make_sentinel ());
          }
        ++m_size;
        return current;
      }

      template <typename ArgType>
      NodeType* preerase(const ArgType& key)
      {
        if (!m_root)
          return nullptr;

        auto current = static_cast<NodeType *> (m_root.get());
        while (current)
        {
          if (key < current->key())
            current = current->child(left);
          else if (key > current->key())
            current = current->child(right);
          else
            break;
        }

        if (!current)
          return nullptr;

        --m_size;

        auto cnt = current->child_count();
        if (cnt <= 1)
          return current;

        auto child = current->child(left);
        while (auto next_child = child->child(right))
          child = next_child;

        current->m_key = child->m_key;
        return child;
      }

    private:
      node_storage_t make_sentinel ()
      {
        return node_storage_t (&this->m_sentinel, [](base_node_t*){});
      }

    private:
      sentinel_t m_sentinel;
      size_t m_size;
      std::array<NodeType *, 2> m_furthest_node = {nullptr, nullptr};
      node_storage_t m_root;

      friend struct rb_tree_container<KeyType>;
      friend class iterator_t;
    };
  };

  template <typename KeyType, template<typename> class... Plugins>
  struct rb_tree_container
  {
    class tree_t;

    struct node_t : btree_container<KeyType, node_t, Plugins...>::node_t
    {
      using parent_t = typename btree_container<KeyType, node_t, Plugins...>::node_t;
      using parent_t::parent_t;
    public:
      auto color() const { return m_color; }

      auto child_color(direction_t direction) const
      {
        if (!this->child(direction))
          return color_t::black;

        return this->child(direction)->color();
      }

    private:
      color_t m_color = color_t::red;
      void paint(color_t value) { m_color = value; }

      friend class tree_t;
    };

    class tree_t : public btree_container<KeyType, node_t, Plugins...>::tree_t
    {
      // red black tree with indexation by log n
      using self = tree_t;
      using base = typename btree_container<KeyType, node_t, Plugins...>::tree_t;
    public:
      using node_t = node_t;
    public:

      tree_t () : base () {}
      ~tree_t () {}

      template <typename ArgType>
      void insert(ArgType&& key)
      {
        auto current = this->preinsert(std::forward<ArgType>(key));
        while (true)
        {
          auto parent = current->parent();
          if (!parent)
          {
            current->paint(color_t::black);
            break;
          }

          if (parent->m_color == color_t::black)
            break;

          auto grand_parent = parent->parent();
          auto uncle = current->uncle();
          if (uncle && uncle->m_color == color_t::red)
          {
            parent->paint(color_t::black);
            uncle->paint(color_t::black);
            grand_parent->paint(color_t::red);
            current = grand_parent;
            continue;
          }

          if (current->direction_from_parent() != parent->direction_from_parent())
          {
            this->rotate(parent, other_direction(current->direction_from_parent()));
            std::swap(current, parent);
          }

          this->rotate(grand_parent, other_direction(current->direction_from_parent()));
          std::swap(grand_parent->m_color, parent->m_color);
          break;
        }
      }

      template <typename ArgType>
      std::size_t erase(const ArgType& key)
      {
        auto m = this->preerase(key);
        if (!m)
          return 0;

        if (m->color() == color_t::red)
          {
            replace_with_child(m);
            return 1;
          }
        auto c = m->single_child();
        auto p = m->parent();
        auto s = m->sibling();
        auto n_direction = m->direction_from_parent();
        replace_with_child(m);
        if (c && c->color() == color_t::red)
        {
          c->paint(color_t::black);
          return 1;
        }

        auto n = c;
        while (true)
        {
          if (p == nullptr)
            return 1;

          if (s->color() == color_t::red)
          {
            std::swap(p->m_color, s->m_color);
            this->rotate(p, n_direction);
            s = p->child(other_direction(n_direction)); // update sibling
          }

          if (s->child_color(left) == color_t::black
            && s->child_color(right) == color_t::black)
          {
            if (p->color() == color_t::black)
            {
              s->paint(color_t::red);
              n = p;
              p = n->parent();
              s = n->sibling();
              n_direction = p ? n->direction_from_parent() : none;
              continue;
            }

            std::swap(s->m_color, p->m_color);
            break;
          }
          if (s->child_color(n_direction) == color_t::red)
          {
            this->rotate(s, other_direction(n_direction));
            std::swap(s->m_color, s->parent()->m_color);
            s = s->parent();
          }

          this->rotate(p, n_direction);
          std::swap(p->m_color, s->m_color);
          s->child(other_direction(n_direction))->paint(color_t::black);
          break;
        }
        return 1;
      }
    };
  };

  template <typename KeyType>
  using rb_tree = typename rb_tree_container<KeyType>::tree_t;
  template <typename KeyType>
  using indexed_rb_tree = typename rb_tree_container<KeyType, indexation_plugin>::tree_t;
} // namespace btrees
