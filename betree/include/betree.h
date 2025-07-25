#ifndef betree_h
#define betree_h

/* Generated with cbindgen:0.27.0 */

/* Warning, this file is autogenerated by cbindgen. Don't modify this manually. */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/**
 * Internal block size (4KiB)
 */
#define BLOCK_SIZE 4096

#define BUFFER_STATIC_SIZE HEADER

#define CHUNK_MAX (UINT32_MAX - 1024)

/**
 * 128 kibibyte
 */
#define CHUNK_SIZE (128 * 1024)

/**
 * The amount of storage classes.
 */
#define NUM_STORAGE_CLASSES 4

/**
 * 256KiB, so that `vdev::BLOCK_SIZE * SEGMENT_SIZE == 1GiB`
 */
#define SEGMENT_SIZE (1 << SEGMENT_SIZE_LOG_2)

/**
 * Number of bytes required to store a segments allocation bitmap
 */
#define SEGMENT_SIZE_BYTES (SEGMENT_SIZE / 8)

/**
 * A byte slice reference counter
 */
typedef struct byte_slice_rc_t byte_slice_rc_t;

/**
 * The type for a database configuration
 */
typedef struct cfg_t cfg_t;

/**
 * The database type
 */
typedef struct db_t db_t;

/**
 * The data set type
 */
typedef struct ds_t ds_t;

/**
 * The general error type
 */
typedef struct err_t err_t;

/**
 * The data set/snapshot name iterator type
 */
typedef struct name_iter_t name_iter_t;

/**
 * The object store wrapper type
 */
typedef struct obj_store_t obj_store_t;

/**
 * The handle of an object in the corresponding object store
 */
typedef struct obj_t obj_t;

/**
 * The range iterator type
 */
typedef struct range_iter_t range_iter_t;

/**
 * The snapshot type
 */
typedef struct ss_t ss_t;

/**
 * An allocation preference. If a [StoragePreference] other than [StoragePreference::NONE]
 * is used for an operation, the allocator will try to allocate on that storage class,
 * but success is not guaranteed.
 *
 * A value of [StoragePreference::NONE] implies that the caller doesn't care about which storage
 * class is used, and that the database should fall back to a more general preference, e.g.
 * per-dataset, or the global default.
 *
 * The different class constants are vaguely named `FASTEST`, `FAST`, `SLOW`, and `SLOWEST`,
 * but a [StoragePreference] can also be created with [StoragePreference::new].
 *
 * The exact properties of a storage layer depend on the database administrator, who is assumed
 * to ensure that the vague ordering properties hold for the given deployment.
 *
 * This type is not an `Option<u8>`, because it saves one byte per value, and allows the
 * implementation of convenience methods on itself.
 */
typedef uint8_t StoragePreference;
/**
 * No preference, any other preference overrides this.
 */
#define StoragePreference_NONE NONE
/**
 * The fastest storage class (0).
 */
#define StoragePreference_FASTEST FASTEST
/**
 * The second-fastest storage class (1).
 */
#define StoragePreference_FAST FAST
/**
 * The third-fastest, or second-slowest, storage class (2).
 */
#define StoragePreference_SLOW SLOW
/**
 * The slowest storage class (3).
 */
#define StoragePreference_SLOWEST SLOWEST

/**
 * The storage preference
 */
typedef struct storage_pref_t {
  StoragePreference _0;
} storage_pref_t;

/**
 * A reference counted byte slice
 */
typedef struct byte_slice_t {
  const char *ptr;
  unsigned int len;
  const struct byte_slice_rc_t *arc;
} byte_slice_t;

/**
 * Highest storage preference.
 */
#define STORAGE_PREF_FASTEST (storage_pref_t){ ._0 = StoragePreference_FASTEST }

/**
 * Default storage preference.
 */
#define STORAGE_PREF_NONE (storage_pref_t){ ._0 = StoragePreference_NONE }

/**
 * Lowest storage preference.
 */
#define STORAGE_PREF_SLOWEST (storage_pref_t){ ._0 = StoragePreference_SLOWEST }

/**
 * Build a database given by a configuration.
 *
 * On success, return a `db_t` which has to be freed with `betree_close_db`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 */
struct db_t *betree_build_db(const struct cfg_t *cfg, struct err_t **err);

/**
 * Closes a database.
 *
 * Note that the `db_t` may not be used afterwards.
 */
void betree_close_db(struct db_t *db);

/**
 * Close a data set.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that the `ds_t` may not be used afterwards.
 */
int betree_close_ds(struct db_t *db, struct ds_t *ds, struct err_t **err);

/**
 * Parse configuration from file specified in environment (BETREE_CONFIG).
 *
 * On success, return a `cfg_t` which has to be freed with `betree_free_cfg`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 */
struct cfg_t *betree_configuration_from_env(struct err_t **err);

/**
 * Resets the access modes for all applicable vdevs to 'Direct'.
 */
void betree_configuration_set_direct(struct cfg_t *cfg, int32_t direct);

/**
 * Reconfigures the given configuration to use a single tier with the given path as the sole backing disk.
 */
void betree_configuration_set_disks(struct cfg_t *cfg,
                                    const char *const *paths,
                                    uintptr_t num_disks);

/**
 * Sets the storage pools disk-dependent iodepth.
 */
void betree_configuration_set_iodepth(struct cfg_t *cfg, uint32_t iodepth);

/**
 * Create a database given by a configuration.
 *
 * On success, return a `db_t` which has to be freed with `betree_close_db`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 *
 * Note that any existing database will be overwritten!
 */
struct db_t *betree_create_db(const struct cfg_t *cfg, struct err_t **err);

/**
 * Create a new data set with the given name.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that the creation fails if a data set with same name exists already.
 */
int betree_create_ds(struct db_t *db,
                     const char *name,
                     unsigned int len,
                     struct storage_pref_t storage_pref,
                     struct err_t **err);

/**
 * Create an object store.
 */
struct obj_store_t *betree_create_object_store(struct db_t *db,
                                               const char *name,
                                               unsigned int name_len,
                                               struct storage_pref_t storage_pref,
                                               struct err_t **err);

/**
 * Create a new snapshot for the given data set with the given name.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that the creation fails if a snapshot with same name exists already
 * for this data set.
 */
int betree_create_snapshot(struct db_t *db,
                           struct ds_t *ds,
                           const char *name,
                           unsigned int len,
                           struct err_t **err);

/**
 * Delete the value for the given `key` if the key exists.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 */
int betree_dataset_delete(const struct ds_t *ds,
                          const char *key,
                          unsigned int len,
                          struct err_t **err);

/**
 * Retrieve the `value` for the given `key`.
 *
 * On success, return 0.  If the key does not exist, return -1.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that on success `value` has to be freed with `betree_free_byte_slice`.
 */
int betree_dataset_get(const struct ds_t *ds,
                       const char *key,
                       unsigned int len,
                       struct byte_slice_t *value,
                       struct err_t **err);

/**
 * Insert the given key-value pair.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that any existing value will be overwritten.
 */
int betree_dataset_insert(const struct ds_t *ds,
                          const char *key,
                          unsigned int key_len,
                          const char *data,
                          unsigned int data_len,
                          struct storage_pref_t storage_pref,
                          struct err_t **err);

/**
 * Return the data set's name.
 */
const char *betree_dataset_name(const struct ds_t *ds, unsigned int *len);

/**
 * Iterate over all key-value pairs in the given key range.
 * `low_key` is inclusive, `high_key` is exclusive.
 *
 * On success, return a `range_iter_t` which has to be freed with
 * `betree_free_range_iter`. On error, return null.  If `err` is not null,
 * store an error in `err`.
 */
struct range_iter_t *betree_dataset_range(const struct ds_t *ds,
                                          const char *low_key,
                                          unsigned int low_key_len,
                                          const char *high_key,
                                          unsigned int high_key_len,
                                          struct err_t **err);

/**
 * Delete all key-value pairs in the given key range.
 * `low_key` is inclusive, `high_key` is exclusive.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 */
int betree_dataset_range_delete(const struct ds_t *ds,
                                const char *low_key,
                                unsigned int low_key_len,
                                const char *high_key,
                                unsigned int high_key_len,
                                struct err_t **err);

/**
 * Upsert the value for the given key at the given offset.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that the value will be zeropadded as needed.
 */
int betree_dataset_upsert(const struct ds_t *ds,
                          const char *key,
                          unsigned int key_len,
                          const char *data,
                          unsigned int data_len,
                          unsigned int offset,
                          struct storage_pref_t storage_pref,
                          struct err_t **err);

/**
 * Delete the snapshot for the given data set with the given name.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that the deletion fails if a snapshot with the given name does not
 * exist for this data set.
 */
int betree_delete_snapshot(struct db_t *db,
                           struct ds_t *ds,
                           const char *name,
                           unsigned int len,
                           struct err_t **err);

/**
 * Free a byte slice.
 */
void betree_free_byte_slice(struct byte_slice_t *x);

/**
 * Free a configuration object.
 */
void betree_free_cfg(struct cfg_t *cfg);

/**
 * Free an error object.
 */
void betree_free_err(struct err_t *err);

/**
 * Free a data set/snapshot name iterator.
 */
void betree_free_name_iter(struct name_iter_t *name_iter);

/**
 * Free a range iterator.
 */
void betree_free_range_iter(struct range_iter_t *range_iter);

/**
 * Enable the global env_logger, configured via environment variables.
 */
void betree_init_env_logger(void);

/**
 * Iterate over all data sets of a database.
 *
 * On success, return a `name_iter_t` which has to be freed with
 * `betree_free_name_iter`. On error, return null.  If `err` is not null,
 * store an error in `err`.
 */
struct name_iter_t *betree_iter_datasets(struct db_t *db, struct err_t **err);

/**
 * Iterate over all snapshots of a data set.
 *
 * On success, return a `name_iter_t` which has to be freed with
 * `betree_free_name_iter`. On error, return null.  If `err` is not null,
 * store an error in `err`.
 */
struct name_iter_t *betree_iter_snapshots(const struct db_t *db,
                                          const struct ds_t *ds,
                                          struct err_t **err);

/**
 * Save the next item in the iterator in `name`.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that `name` may not be used on error but on success,
 * it has to be freed with `betree_free_byte_slice` afterwards.
 */
int betree_name_iter_next(struct name_iter_t *name_iter,
                          struct byte_slice_t *name,
                          struct err_t **err);

/**
 * Closes an object. The handle may not be used afterwards.
 */
int betree_object_close(struct obj_t *obj, struct err_t **err);

/**
 * Create a new object.
 */
struct obj_t *betree_object_create(struct obj_store_t *os,
                                   const char *key,
                                   unsigned int key_len,
                                   struct storage_pref_t storage_pref,
                                   struct err_t **err);

/**
 * Delete an existing object. The handle may not be used afterwards.
 */
int betree_object_delete(struct obj_t *obj, struct err_t **err);

/**
 * Fetch the size of the given object if it exists. Returns 0 on error.
 */
unsigned long long betree_object_get_size(struct obj_store_t *os,
                                          const char *key,
                                          unsigned int key_len,
                                          struct err_t **err);

/**
 * Open an existing object.
 */
struct obj_t *betree_object_open(struct obj_store_t *os,
                                 const char *key,
                                 unsigned int key_len,
                                 struct storage_pref_t storage_pref,
                                 struct err_t **err);

/**
 * Try to open an existing object, create it if none exists.
 */
struct obj_t *betree_object_open_or_create(struct obj_store_t *os,
                                           const char *key,
                                           unsigned int key_len,
                                           struct storage_pref_t storage_pref,
                                           struct err_t **err);

/**
 * Try to read `buf_len` bytes of `obj` into `buf`, starting at `offset` bytes into the objects
 * data. The actually read number of bytes is written into `n_read` if and only if the read
 * succeeded.
 */
int betree_object_read_at(struct obj_t *obj,
                          char *buf,
                          unsigned long buf_len,
                          unsigned long offset,
                          unsigned long *n_read,
                          struct err_t **err);

/**
 * Try to write `buf_len` bytes from `buf` into `obj`, starting at `offset` bytes into the objects
 * data.
 */
int betree_object_write_at(struct obj_t *obj,
                           const char *buf,
                           unsigned long buf_len,
                           unsigned long offset,
                           unsigned long *n_written,
                           struct err_t **err);

/**
 * Open a database given by a configuration. If no initialized database is present this procedure will fail.
 *
 * On success, return a `db_t` which has to be freed with `betree_close_db`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 */
struct db_t *betree_open_db(const struct cfg_t *cfg,
                            struct err_t **err);

/**
 * Open a data set identified by the given name.
 *
 * On success, return a `ds_t` which has to be freed with `betree_close_ds`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 */
struct ds_t *betree_open_ds(struct db_t *db,
                            const char *name,
                            unsigned int len,
                            struct storage_pref_t storage_pref,
                            struct err_t **err);

/**
 * Create a database given by a configuration.
 *
 * On success, return a `db_t` which has to be freed with `betree_close_db`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 *
 * Note that any existing database will be overwritten!
 */
struct db_t *betree_open_or_create_db(const struct cfg_t *cfg, struct err_t **err);

/**
 * Parse the configuration string for a storage pool.
 *
 * On success, return a `cfg_t` which has to be freed with `betree_free_cfg`.
 * On error, return null.  If `err` is not null, store an error in `err`.
 */
struct cfg_t *betree_parse_configuration(const char *const *cfg_strings,
                                         unsigned int cfg_strings_len,
                                         struct err_t **err);

/**
 * Print the given error to stderr.
 */
void betree_print_error(struct err_t *err);

/**
 * Save the next key-value pair in the iterator.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that `key` and `value` may not be used on error but on success,
 * both have to be freed with `betree_free_byte_slice` afterwards.
 */
int betree_range_iter_next(struct range_iter_t *range_iter,
                           struct byte_slice_t *key,
                           struct byte_slice_t *value,
                           struct err_t **err);

/**
 * Retrieve the `value` for the given `key`.
 *
 * On success, return 0.  If the key does not exist, return -1.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 *
 * Note that on success `value` has to be freed with `betree_free_byte_slice`.
 */
int betree_snapshot_get(const struct ss_t *ss,
                        const char *key,
                        unsigned int len,
                        struct byte_slice_t *value,
                        struct err_t **err);

/**
 * Iterate over all key-value pairs in the given key range.
 * `low_key` is inclusive, `high_key` is exclusive.
 *
 * On success, return a `range_iter_t` which has to be freed with
 * `betree_free_range_iter`. On error, return null.  If `err` is not null,
 * store an error in `err`.
 */
struct range_iter_t *betree_snapshot_range(const struct ss_t *ss,
                                           const char *low_key,
                                           unsigned int low_key_len,
                                           const char *high_key,
                                           unsigned int high_key_len,
                                           struct err_t **err);

/**
 * Sync a database.
 *
 * On success, return 0.
 * On error, return -1.  If `err` is not null, store an error in `err`.
 */
int betree_sync_db(struct db_t *db, struct err_t **err);

#endif  /* betree_h */
