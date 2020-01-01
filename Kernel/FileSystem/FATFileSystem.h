/**
 * FAT File System Driver
 * 
 * Currently only supports FAT32 but can be extended to support FAT12/FAT16
 * 
 * This driver currently only supports FAT32.
 * obviously bad 
 * 
 * Reference:   https://github.com/ryansturmer/thinfat32/blob/master/fatgen103.pdf
 *              https://github.com/adafruit/SD/blob/master/utility/FatStructs.h
 *              http://www.c-jump.com/CIS24/Slides/FAT/FAT.html
 * 
 * Author: Jesse Buhagiar [quaker762]
 */
#pragma once

#include <cstdint> // Is this naughty, kling???

#include "FileSystem/DiskBackedFileSystem.h"
#include "FileSystem/Inode.h"

// FIXME: This can be a union with two nested structs to take into
// account the differences between the FAT12/16 and FAT32 versions
struct __attribute__((packed)) msdos_bios_parameter_block
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t number_of_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors16;
    uint8_t media_type;
    uint16_t fat_size16;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t number_of_hidden_sectors;
    uint32_t total_sectors32;

    // These are FAT32 specific variables!
    uint32_t fat_size32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fsinfo_ptr;
    uint16_t backup_boot_ptr;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1; // This name is to remain consistent with Fatgen103
    uint8_t boot_signature;
    uint32_t volume_id;
    char volume_label[11];
    char fs_type[8]; // This is _always_ "FAT32   "
};

// Boot Sector (VBR) of a FAT volume
struct __attribute__((packed)) msdos_vbr
{
    uint8_t jmp[3];
    char oem_name[8];
    struct msdos_bios_parameter_block;
    uint16_t boot_signature; // 0xAA55
};

class FATFileSystem;

class FATInode : public Inode
{
    friend class FATFileSystem;

private:
    // ^Inode
    virtual ssize_t read_bytes(off_t, ssize_t, u8* buffer, FileDescription*) const override;
    virtual InodeMetadata metadata() const override;
    virtual bool traverse_as_directory(Function<bool(const FS::DirectoryEntry&)>) const override;
    virtual InodeIdentifier lookup(StringView name) override;
    virtual void flush_metadata() override;
    virtual ssize_t write_bytes(off_t, ssize_t, const u8* data, FileDescription*) override;
    virtual KResult add_child(InodeIdentifier child_id, const StringView& name, mode_t) override;
    virtual KResult remove_child(const StringView& name) override;
    virtual int set_atime(time_t) override;
    virtual int set_ctime(time_t) override;
    virtual int set_mtime(time_t) override;
    virtual int increment_link_count() override;
    virtual int decrement_link_count() override;
    virtual size_t directory_entry_count() const override;
    virtual KResult chmod(mode_t) override;
    virtual KResult chown(uid_t, gid_t) override;
    virtual KResult truncate(off_t) override;
};

class FATFileSystem : public DiskBackedFS
{
    friend class FATInode;

public:
    static NonnullRefPtr<FATFileSystem> create(NonnullRefPtr<DiskDevice>);
    virtual ~FATFileSystem() override;
    virtual bool initialize() override;

    virtual unsigned total_block_count() const override;
    virtual unsigned free_block_count() const override;
    virtual unsigned total_inode_count() const override;
    virtual unsigned free_inode_count() const override;

    virtual KResult prepare_to_unmount() const override;

private:
    struct msdos_vbr m_volume_boot_record;
};