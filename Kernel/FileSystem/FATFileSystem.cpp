/**
 * 
 */
#include "FATFileSystem.h"

#define FAT_DEBUG

bool FATFileSystem::initialize()
{
    LOCKER(m_lock); // Acquire the fs lock
    device().read_block(0, reinterpret_cast<u8*>(&m_volume_boot_record));
    ASSERT(m_volume_boot_record.boot_signature == 0xaa55);

    
    return true;
}