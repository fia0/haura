//! This module provides a B<sup>e</sup>-Tree on top of the Data Management
//! Layer.

mod default_message_action;
mod errors;
pub mod imp;
mod layer;
mod message_action;
mod pivot_key;

use serde::{Deserialize, Serialize};

use crate::cow_bytes::{CowBytes, SlicedCowBytes};

pub use self::{
    default_message_action::DefaultMessageAction,
    imp::{Inner, Node, Tree},
    layer::TreeLayer,
    message_action::MessageAction,
};

#[repr(C)]
#[derive(Debug, Clone, Copy, Hash, PartialEq, Eq, Serialize, Deserialize)]
/// Which node representation the tree should use.
pub enum StorageKind {
    /// Conventional large nodes. HDD optimized.
    Hdd = 0,
    /// Partially fetched nodes. Memory only.
    Memory,
    /// Segmented nodes. For fast SSDs.
    Ssd,
}

impl Default for StorageKind {
    fn default() -> Self {
        Self::Hdd
    }
}

#[cfg(not(feature = "internal-api"))]
pub(crate) use self::pivot_key::PivotKey;

#[cfg(feature = "internal-api")]
pub use self::{imp::NodeInfo, pivot_key::PivotKey};

type Key = CowBytes;
type Value = SlicedCowBytes;

use self::imp::KeyInfo;
pub(crate) use self::{errors::Error, imp::MAX_MESSAGE_SIZE, layer::ErasedTreeSync};
