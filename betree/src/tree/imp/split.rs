//! Encapsulating logic for splitting of normal and root nodes.
use super::{Inner, Node, Tree};
use crate::{
    cache::AddSize,
    data_management::{Dml, HasStoragePreference, ObjectReference},
    size::Size,
    tree::imp::internal::TakeChildBuffer,
    tree::{errors::*, MessageAction},
};
use std::borrow::Borrow;

impl<X, R, M, I> Tree<X, M, I>
where
    X: Dml<Object = Node<R>, ObjectRef = R>,
    R: ObjectReference<ObjectPointer = X::ObjectPointer> + HasStoragePreference,
    M: MessageAction,
    I: Borrow<Inner<X::ObjectRef, M>>,
{
    pub(super) fn split_root_node(&self, mut root_node: X::CacheValueRefMut) {
        self.dml.verify_cache();
        let before = root_node.cache_size();
        debug!(
            "Splitting root. {}, {:?}, {}, {:?}",
            root_node.kind(),
            root_node.fanout(),
            root_node.size(),
            root_node.actual_size()
        );
        let size_delta = root_node.split_root_mut(&self.storage_map, |node, pk| {
            debug!(
                "Root split child: {}, {:?}, {}, {:?}",
                node.kind(),
                node.fanout(),
                node.size(),
                node.actual_size()
            );
            self.dml
                .insert(node, self.tree_id(), pk.to_global(self.tree_id()))
        });
        info!("Root split done. {}, {}", root_node.size(), size_delta);
        assert!(before as isize + size_delta == root_node.cache_size() as isize);
        root_node.finish(size_delta);
        self.dml.verify_cache();
    }

    pub(super) fn split_node(
        &self,
        mut node: X::CacheValueRefMut,
        parent: &mut TakeChildBuffer<R>,
    ) -> Result<(X::CacheValueRefMut, isize), Error> {
        self.dml.verify_cache();

        let before = node.cache_size();
        let (sibling, pivot_key, size_delta, lpk) = node.split(&self.storage_map);
        let pk = lpk.to_global(self.tree_id());
        let select_right = sibling.size() > node.size();
        debug!(
            "split {}: {} -> ({}, {}), {}",
            node.kind(),
            before,
            node.size(),
            sibling.size(),
            select_right,
        );
        node.add_size(size_delta);
        let sibling_np = if select_right {
            let (sibling, np) = self.dml.insert_and_get_mut(sibling, self.tree_id(), pk);
            node = sibling;
            np
        } else {
            self.dml.insert(sibling, self.tree_id(), pk)
        };

        let size_delta = parent.split_child(sibling_np, pivot_key, select_right);

        Ok((node, size_delta))
    }
}
