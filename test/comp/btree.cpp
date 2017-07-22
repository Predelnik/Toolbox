#include "comp/btree.h"
#include "catch.hpp"

template <typename Tree>
struct tree_walker
{
private:
  using self = tree_walker;
public:
  tree_walker (const Tree &tree) : m_tree (&tree) { m_cur_node = m_tree->root (); }
  self &color (btree::color_t expected)
  {
    REQUIRE (m_cur_node->color () == expected);
    return *this;
  }

  template <typename ArgType>
  self &key (const ArgType &expected)
  {
    REQUIRE (m_cur_node->key () == expected);
    return *this;
  }

  self &walk (btree::direction_t direction)
  {
    m_cur_node = m_cur_node->child (direction);
    return *this;
  }

  self &up ()
  {
    m_cur_node = m_cur_node->parent ();
    return *this;
  }

  self &to_root ()
  {
    m_cur_node = m_tree->root ();
    return *this;
  }

private:
  const typename Tree::node_t *m_cur_node;
  const Tree *m_tree;
};

template <typename Tree>
tree_walker<Tree> walk (const Tree &tree) { return {tree}; }

TEST_CASE("rb_tree") {
  using namespace btree;
  rb_tree<int> t;
  for (auto i : {5, 11, 3, 16, 12})
    t.insert (i);
  walk (t).color(color_t::black).key (5)
    .walk(left).key (3).color (color_t::black)
  .up ().walk(right).key (12).color (color_t::black)
  .walk(left).key (11).color (color_t::red)
  .up ().walk(right).key (16).color (color_t::red);
  t.clear ();
  for (auto i : {5, 11, 3, 16, 12, 7, 66, 15, 10})
    t.insert (i);

  for (auto i : {12, 3, 66})
    t.erase (i);

  walk (t).color(color_t::black).key (11).color (color_t::black)
    .walk(left).key (7).color (color_t::red)
  .walk(left).key (5).color (color_t::black)
  .up ().walk(right).key (10).color (color_t::black)
  .to_root()
  .walk(right).key (16).color (color_t::black)
  .walk(left).key (15).color (color_t::red);
}

TEST_CASE("indexed_rb_tree")
{
  using namespace btree;
  indexed_rb_tree<int> tree;
  tree.insert(3);
  REQUIRE (tree.index (3) == 0);
  tree.insert(7);
  REQUIRE (tree.index (7) == 1);
  tree.insert(5);
  REQUIRE (tree.index (5) == 1);
  tree.insert(6);
  REQUIRE (tree.index (6) == 2);
  REQUIRE (tree.index (3) == 0);
  tree.erase (5);
  REQUIRE (tree.index (6) == 1);
  REQUIRE (tree.index (7) == 2);
  tree.insert (100);
  tree.insert (150);
  tree.insert (200);
  REQUIRE (tree.index (150) == 4);
}