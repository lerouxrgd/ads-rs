//! A cyclic list-based queue

use std::cell::RefCell;
use std::rc::Rc;

pub struct Queue<T> {
    entry: NodePtr<T>,
}

struct Node<T> {
    elem: Option<T>,
    next: Link<T>,
}

type Link<T> = Option<NodePtr<T>>;

struct NodePtr<T>(Rc<RefCell<Node<T>>>);

impl<T> NodePtr<T> {
    fn new(node: Node<T>) -> NodePtr<T> {
        NodePtr(Rc::new(RefCell::new(node)))
    }

    fn next(&self) -> NodePtr<T> {
        (*self.0.borrow()).next.as_ref().expect("no next").clone()
    }

    fn set_next(&mut self, next: NodePtr<T>) {
        (*self.0.borrow_mut()).next = Some(next);
    }

    fn take_next(&mut self) -> Option<NodePtr<T>> {
        (*self.0.borrow_mut()).next.take()
    }

    fn take_val(&mut self) -> Option<T> {
        (*self.0.borrow_mut()).elem.take()
    }

    fn map_val<R>(&self, f: impl Fn(&T) -> R) -> Option<R> {
        match (*self.0.borrow()).elem.as_ref() {
            Some(elem) => Some(f(elem)),
            None => None,
        }
    }

    fn update_val(&self, f: impl Fn(&mut T)) -> Option<()> {
        match (*self.0.borrow_mut()).elem.as_mut() {
            Some(elem) => Some(f(elem)),
            None => None,
        }
    }
}

impl<T> Clone for NodePtr<T> {
    fn clone(&self) -> Self {
        NodePtr(self.0.clone())
    }
}

impl<T> PartialEq for NodePtr<T> {
    fn eq(&self, other: &Self) -> bool {
        Rc::ptr_eq(&self.0, &other.0)
    }
}

impl<T> Queue<T> {
    pub fn new() -> Self {
        let mut entry = NodePtr::new(Node {
            elem: None,
            next: None,
        });

        let mut placeholder = NodePtr::new(Node {
            elem: None,
            next: None,
        });

        entry.set_next(placeholder.clone());
        placeholder.set_next(placeholder.clone());

        Queue { entry }
    }

    pub fn is_empty(&self) -> bool {
        self.entry.next() == self.entry.next().next()
    }

    pub fn push(&mut self, elem: T) {
        let mut new_node = NodePtr::new(Node {
            elem: Some(elem),
            next: None,
        });

        let mut tmp = self.entry.next();
        self.entry.set_next(new_node.clone());
        new_node.set_next(tmp.next());
        tmp.set_next(new_node.clone());
    }

    pub fn pop(&mut self) -> Option<T> {
        if self.is_empty() {
            None
        } else {
            let mut tmp = self.entry.next().next().next();
            self.entry.next().next().set_next(tmp.next());
            if tmp == self.entry.next() {
                self.entry.set_next(tmp.next());
            }
            tmp.take_val()
        }
    }

    pub fn peek_map<R>(&self, f: impl Fn(&T) -> R) -> Option<R> {
        self.entry.next().next().next().map_val(f)
    }

    pub fn peek_update(&self, f: impl Fn(&mut T)) -> Option<()> {
        self.entry.next().next().next().update_val(f)
    }

    pub fn into_iter(mut self) -> impl Iterator<Item = T> {
        std::iter::from_fn(move || self.pop())
    }

    pub fn iter_map<R>(&self, f: impl Fn(&T) -> R) -> impl Iterator<Item = R> {
        let mut next = self.entry.next().next().next();

        std::iter::from_fn(move || {
            let res = next.map_val(&f);
            next = next.next();
            res
        })
    }

    pub fn for_each(&mut self, f: impl Fn(&mut T)) {
        let mut next = self.entry.next().next().next();
        while let Some(()) = next.update_val(&f) {
            next = next.next();
        }
    }
}

impl<T> Drop for Queue<T> {
    fn drop(&mut self) {
        let mut tmp = self.entry.next().next();
        while let Some(next) = tmp.take_next() {
            tmp = next;
        }
        self.entry.take_next().expect("unreachable");
    }
}

#[cfg(test)]
mod test {
    use super::Queue;

    #[test]
    fn basics() {
        let mut queue = Queue::new();

        // Check empty queue behaves right
        assert_eq!(queue.pop(), None);

        // Populate queue
        queue.push(1);
        queue.push(2);
        queue.push(3);

        // Check normal removal
        assert_eq!(queue.pop(), Some(1));
        assert_eq!(queue.pop(), Some(2));

        // Push some more just to make sure nothing's corrupted
        queue.push(4);
        queue.push(5);

        // Check normal removal
        assert_eq!(queue.pop(), Some(3));
        assert_eq!(queue.pop(), Some(4));

        // Check exhaustion
        assert_eq!(queue.pop(), Some(5));
        assert_eq!(queue.pop(), None);

        // Check the exhaustion case fixed the pointer right
        queue.push(6);
        queue.push(7);
        queue.push(8);

        // Check normal removal and peeking
        assert_eq!(queue.pop(), Some(6));
        assert_eq!(queue.peek_map(|val| *val + 1), Some(8));
        assert_eq!(queue.peek_update(|val| *val = *val + 1), Some(()));
        assert_eq!(queue.pop(), Some(8));
        assert_eq!(queue.pop(), Some(8));
        assert_eq!(queue.pop(), None);
        assert_eq!(queue.peek_map(|val| *val + 1), None);
        assert_eq!(queue.peek_update(|val| *val = *val + 1), None);
    }

    #[test]
    fn into_iter() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        let mut iter = queue.into_iter();
        assert_eq!(iter.next(), Some(1));
        assert_eq!(iter.next(), Some(2));
        assert_eq!(iter.next(), Some(3));
        assert_eq!(iter.next(), None);
    }

    #[test]
    fn iter_map() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        let mut iter = queue.iter_map(|v| *v);
        assert_eq!(iter.next(), Some(1));
        assert_eq!(iter.next(), Some(2));
        assert_eq!(iter.next(), Some(3));
        assert_eq!(iter.next(), None);
    }

    #[test]
    fn for_each() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        queue.for_each(|v| *v = *v + 1);
        let mut iter = queue.into_iter();
        assert_eq!(iter.next(), Some(2));
        assert_eq!(iter.next(), Some(3));
        assert_eq!(iter.next(), Some(4));
        assert_eq!(iter.next(), None);
    }
}
