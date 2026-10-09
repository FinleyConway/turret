#pragma once

#include <vector>
#include <cassert>
#include <sys/mman.h>

namespace turret {
    class mapping {
    public:
        mapping(int fd, size_t length, size_t offset) : 
            m_fd(fd),
            m_length(length) 
        {
            void* address = mmap(
                nullptr,
                length,
                PROT_READ, MAP_SHARED,
                fd,
                offset
            );

            assert(address != MAP_FAILED);

            m_address = address;
        }

        ~mapping() {
            if (m_address != MAP_FAILED) {
                munmap(m_address, m_length);
            }
        }

    public:
        int fd() const {
            return m_fd;
        }

        void* data() const {
            return m_address;
        }

    private:
        int m_fd;
        void* m_address = MAP_FAILED;
        size_t m_length = 0;
    };

    class mapped_framebuffer {
    public:
        void add_mapping(int fd, size_t length, size_t offset) {
            m_mappings.emplace_back(fd, length, offset);
        }

        void* get_mapping(int fd) {
            for (const auto& map : m_mappings) {
                if (map.fd() == fd) {
                    return map.data();
                }
            }

            return nullptr;
        }

    private:
        std::vector<mapping> m_mappings;
    };
}