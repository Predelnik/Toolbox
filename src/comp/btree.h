#pragma once
#include <memory>
#include <map>
#include <cassert>
#include <array>

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
    left,
    right,
  };

  inline direction_t other_direction(direction_t d)
  {
    return d == left ? right : left;
  }

  template <typename KeyType, template <typename, typename> class... Plugins>
  struct rb_tree_container;

  template <typename KeyType, typename NodeType, template <typename, typename> class... Plugins>
  struct btree_container;

  template <typename KeyType, typename NodeType>
  class indexation_plugin
    {
    public:
      class node_t
      {
      public:
          std::size_t node_count () const { return m_node_count; }

      private:
        std::size_t m_node_count = 1;
        
      public:
        void on_new_child ()
        {
          auto current = static_cast<NodeType *> (this)->parent();
          while (current) 
          {
            current->m_node_count += m_node_count;
            current = current->parent ();
          }
        }

        void before_takeout ()
        {
          auto current = static_cast<NodeType *> (this)->parent();
          while (current) 
          {
            current->m_node_count -= m_node_count;
            current = current->parent ();
          }
        }
      };

      template <typename TreeType>
      class tree_t
      {
      public:
        template <typename ArgType>
        std::size_t index (const ArgType &key)
        {
          auto current = static_cast<TreeType*> (this)->root();
          std::size_t index_so_far = 0;
          while (current)
            {
              if (key < current->key ())
                current = current->child (left);
              else if (current->key () < key)
                {
                  index_so_far += (current->child (left) ? current->child (left)->node_count () :0) + 1;
                  current = current->child (right);
                }
              else
                {
                  index_so_far += (current->child (left) ? current->child (left)->node_count () :0);
                  break;
                }
            }
          if (current)
            return index_so_far;

          return static_cast<std::size_t> (-1);
        }
      };
    };

  template <typename KeyType, typename NodeType, template <typename, typename> class... Plugins>
  struct btree_container
  {
    class tree_t;
    class node_t;

  public:
    class node_t : public Plugins<KeyType, NodeType>::node_t...
    {
    public:
      template <typename ArgType>
      explicit node_t(ArgType&& key) : m_key(std::forward<ArgType>(key))
      {
      }

      const auto& key() const { return m_key; }
      const NodeType* child(direction_t direction) const { return m_children[direction].get(); }
      NodeType* child(direction_t direction) { return m_children[direction].get(); }
      const NodeType* parent() const { return m_parent; }
      NodeType* parent() { return m_parent; }

      NodeType* uncle()
      {
        assert (m_parent);
        return m_parent->sibling();
      }

      NodeType* sibling()
      {
        return m_parent->child(other_direction(direction_from_parent()));
      }

      int child_count()
      {
        int cnt = 0;
        for (auto direction : {left, right})
          cnt += (m_children[direction] != nullptr);
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
        assert (m_parent);
        for (auto b : {left, right})
          if (m_parent->m_children[b].get() == this)
            return b;
        assert (false);
        return left;
      }

      std::unique_ptr<NodeType> take_out()
      {
        std::initializer_list<int> {(static_cast<TYPENAME_IF_NOT_MSVC Plugins<KeyType, NodeType>::node_t*> (this)->before_takeout (), 0)...};
        auto& n = location();
        n->m_parent = nullptr;
        return std::move(n);
      }

      NodeType* append_child(direction_t dir, std::unique_ptr<NodeType> node)
      {
        node->m_parent = static_cast<NodeType *>(this);
        assert (m_children[dir] == nullptr);
        m_children[dir] = std::move(node);
        std::initializer_list<int> {(static_cast<TYPENAME_IF_NOT_MSVC Plugins<KeyType, NodeType>::node_t*> (m_children[dir].get ())->on_new_child (),0)...};
        return m_children[dir].get();
      }

    private:
      std::array<std::unique_ptr<NodeType>, 2> m_children;
      NodeType* m_parent = nullptr;
      KeyType m_key;

      std::unique_ptr<NodeType>& location() const
      {
        return m_parent->m_children[this->direction_from_parent()];
      }

      friend class tree_t;
    };

    class tree_t : public Plugins<KeyType, NodeType>::template tree_t<tree_t>...
    {
    public:
      void clear ()
      {
        m_root.reset ();
      }

      std::unique_ptr<NodeType> take_out(NodeType *node)
      {
        if (node->parent ())
          return node->take_out ();
        else
          return std::move (m_root);
      }

      void replace_with_child(NodeType *node)
      {
        auto cnt = node->child_count();
        auto direction = node->direction_from_parent();
        auto parent = node->parent ();
        auto taken_out = take_out (node);
        if (cnt == 0)
          return;
        if (parent)
          parent->append_child(direction, take_out (node->single_child()));
        else
          m_root = node->single_child()->take_out();
      }
      void rotate(NodeType *node, direction_t dir)
      {
        auto prev_parent = node->parent();
        direction_t prev_direction;
        if (prev_parent)
           prev_direction = node->direction_from_parent();
        auto hanging_node = take_out (node);
        auto heritor = take_out (node->child(other_direction(dir)));
        if (heritor->child(dir))
          node->append_child(other_direction(dir), take_out (heritor->child(dir)));

        heritor->append_child(dir, std::move(hanging_node));
        if (prev_parent)
          prev_parent->append_child(prev_direction, std::move(heritor));
        else
          m_root = std::move (heritor);
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

      const NodeType* root() const { return m_root.get(); }

    protected:
      template <typename ArgType>
      NodeType* preinsert(ArgType&& key)
      {
        auto create_node = [](auto&& key)
        {
          auto node = std::make_unique<NodeType>(std::forward<ArgType>(key));
          return node;
        };

        auto current = m_root.get();
        if (!current)
        {
          m_root = create_node(std::forward<ArgType>(key));
          current = m_root.get();
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
        return current;
      }

      template <typename ArgType>
      NodeType* preerase(const ArgType& key)
      {
        if (!m_root)
          return nullptr;

        auto current = m_root.get();
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
      std::unique_ptr<NodeType> m_root;

      friend struct rb_tree_container<KeyType>;
    };
  };

  template <typename KeyType, template<typename, typename> class... Plugins>
  struct rb_tree_container
  {
    class tree_t;

    struct node_t : btree_container<KeyType, node_t, Plugins...>::node_t
    {
      using parent_t = typename btree_container<KeyType, node_t, Plugins...>::node_t;
      using parent_t::parent_t;
    public:
      auto color() const { return m_color; }

      auto child_color(direction_t direction)
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
      template <typename ArgType>
      void insert(ArgType&& key)
      {
        auto current = preinsert(std::forward<ArgType>(key));
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
            rotate(parent, other_direction(current->direction_from_parent()));
            std::swap(current, parent);
          }

          rotate(grand_parent, other_direction(current->direction_from_parent()));
          std::swap(grand_parent->m_color, parent->m_color);
          break;
        }
      }

      template <typename ArgType>
      std::size_t erase(const ArgType& key)
      {
        auto m = preerase(key);
        if (!m)
          return 0;

        if (m->color() == color_t::red)
          return (replace_with_child(m) , 1);
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

        while (true)
        {
          auto n = c;
          if (p == nullptr)
            return 1;

          if (s->color() == color_t::red)
          {
            std::swap(p->m_color, s->m_color);
            rotate(p, n_direction);
            s = p->child (other_direction(n_direction)); // update sibling
          }

          if (s->child_color(left) == color_t::black
            && s->child_color(right) == color_t::black)
          {
            if (p->color() == color_t::black)
            {
              s->paint(color_t::red);
              n = p;
              c = n->single_child();
              p = n->parent();
              s = n->sibling();
              n_direction = n->direction_from_parent();
              continue;
            }

            std::swap(s->m_color, p->m_color);
            break;
          }
          if (s->child_color (n_direction) == color_t::red)
          {
            rotate(s, other_direction(n_direction));
            std::swap(s->m_color, s->parent ()->m_color);
            s = s->parent ();
          }

          rotate(p, n_direction);
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
