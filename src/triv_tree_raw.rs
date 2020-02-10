//! A trivial search tree, without rebalancing

use std::alloc::{alloc, Layout};
use std::marker::PhantomData;
use std::mem;
use std::ptr;

pub struct SearchTree<K, V> {
    tree: *mut TreeNode<K, V>,
    marker: PhantomData<Box<(K, V)>>,
}

struct TreeNode<K, V> {
    key: Key<K>,
    left: *mut TreeNode<K, V>,
    right: *mut TreeNode<K, V>,
}

enum Key<K> {
    Owned(K),
    Ptr(*const K),
}

impl<K> Key<K> {
    fn as_ref(&self) -> &K {
        match self {
            Self::Owned(k) => k,
            Self::Ptr(ptr) => unsafe { &**ptr },
        }
    }
}

impl<K, V> TreeNode<K, V>
where
    K: Ord,
{
    fn new_leaf(key: Key<K>, val: *const V) -> *mut TreeNode<K, V> {
        unsafe {
            // let val_ptr = alloc(Layout::new::<V>()) as *mut V;
            // *val_ptr = val;

            let node = alloc(Layout::new::<TreeNode<K, V>>()) as *mut TreeNode<K, V>;
            (*node).key = key;
            (*node).left = val as *mut _;
            (*node).right = ptr::null_mut();
            node
        }
    }

    fn is_leaf(&self) -> bool {
        self.right.is_null()
    }

    fn val_ptr(&self) -> *const V {
        unsafe { *(self.left as *const _) }
    }

    fn val_mut(&self) -> &mut V {
        unsafe { &mut *(self.left as *mut _) }
    }

    // pub fn data_ref(&self) -> Option<&V> {
    //     self.left.map(|node| unsafe { &*(node.as_ptr() as *mut _) })
    // }
}

impl<K, V> SearchTree<K, V>
where
    K: Ord,
{
    pub fn new() -> SearchTree<K, V> {
        SearchTree {
            tree: ptr::null_mut(),
            marker: PhantomData,
        }
    }

    pub fn insert(&mut self, key: K, val: V) -> Option<V> {
        unsafe {
            if self.tree.is_null() {
                let new_val = alloc(Layout::new::<V>()) as *mut V;
                *new_val = val;
                let tree = TreeNode::new_leaf(Key::Owned(key), new_val);
                self.tree = tree;
                return None;
            }

            let mut tmp_node = self.tree;
            while !(*tmp_node).is_leaf() {
                if &key < (*tmp_node).key.as_ref() {
                    tmp_node = (*tmp_node).left;
                } else {
                    tmp_node = (*tmp_node).right;
                }
            }

            match &mut (*tmp_node).key {
                Key::Owned(_tmp_key) => {
                    //
                    todo!()
                }
                Key::Ptr(tmp_key) => {
                    if &**tmp_key == &key {
                        return Some(mem::replace((*tmp_node).val_mut(), val));
                    }

                    let old_leaf = TreeNode::new_leaf(Key::Ptr(*tmp_key), (*tmp_node).val_ptr());

                    let new_val = alloc(Layout::new::<V>()) as *mut V;
                    *new_val = val;

                    if &**tmp_key < &key {
                        let new_leaf = TreeNode::new_leaf(Key::Ptr(&key as *const _), new_val);
                        (*tmp_node).left = old_leaf;
                        (*tmp_node).right = new_leaf;
                        (*tmp_node).key = Key::Owned(key);
                    } else {
                        let new_leaf = TreeNode::new_leaf(Key::Owned(key), new_val);
                        (*tmp_node).left = new_leaf;
                        (*tmp_node).right = old_leaf;
                    }

                    None
                }
            }
        }
    }
}
