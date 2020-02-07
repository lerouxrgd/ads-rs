//! A trivial search tree, without rebalancing

use std::alloc::{alloc, Layout};
use std::marker::PhantomData;
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
    Val(K),
    Ptr(*const K),
}

impl<K, V> TreeNode<K, V>
where
    K: Ord,
{
    fn new() -> TreeNode<K, V> {
        // let node = alloc(Layout::new::<TreeNode<T>>()) as *mut TreeNode<T>;
        TreeNode {
            key: Key::Ptr(ptr::null()),
            left: ptr::null_mut(),
            right: ptr::null_mut(),
        }
    }
}

impl<K, V> SearchTree<K, V> where K: Ord {}
