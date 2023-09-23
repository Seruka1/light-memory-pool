#include"allocator.h"

#include<cstdint>

#include"macros.h"

namespace arrow
{
    //用于对齐
    constexpr size_t kAlignment = 64;

    //静态内存块用于0-size的分配 alignas是对齐的意思，也就是这块内存的地址位为64的整数倍
    alignas(kAlignment) static uint8_t zero_size_area[1];

    Status SystemAllocator::AllocateAligned(int64_t size, uint8_t **out)
    {
        if(size==0)
        {
            *out = zero_size_area;
            return Status::OK();
        }
        const int result = posix_memalign(reinterpret_cast<void **>(out), kAlignment, static_cast<size_t>(size));
        if(result==ENOMEM)
        {
            return Status::OutOfMemory("malloc of size ", size, " failed");
        }
        if(result==EINVAL)
        {
            return Status::Invalid("invalid alignment parameter ", kAlignment);
        }
        return Status::OK();
    }

    Status SystemAllocator::ReallocateAligned(int64_t old_size, int64_t new_size, uint8_t **ptr)
    {
        uint8_t *previous_ptr = *ptr;
        if(previous_ptr==zero_size_area)
        {
            DCHECK_EQ(old_size, 0);
            return AllocateAligned(new_size, ptr);
        }
        if(new_size==0)
        {
            DeallocateAligned(previous_ptr, old_size);
            *ptr = zero_size_area;
            return Status::OK();
        }
        // Note: We cannot use realloc() here as it doesn't guarantee alignment.

        // Allocate new chunk
        uint8_t *out = nullptr;
        RETURN_NOT_OK(AllocateAligned(new_size, &out));
        DCHECK(out);
        // Copy contents and release old memory chunk
        memcpy(out, *ptr, static_cast<size_t>(std::min(new_size, old_size)));
        free(*ptr);
        *ptr = out;
        return Status::OK();
    }
    void SystemAllocator::DeallocateAligned(uint8_t *ptr, int64_t size)
    {
        if(ptr==zero_size_area)
        {
            DCHECK_EQ(size, 0);
        }
        else
        {
            free(ptr);
        }
    }
    void SystemAllocator::ReleaseUnused()
    {
#ifdef __GLIBC__
        //ARROW_UNUSED(malloc_trim(0));
#endif
    }
}