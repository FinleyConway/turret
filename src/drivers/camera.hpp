#pragma once

#include <libcamera/libcamera.h>

namespace turret {
    class camera {
    public:
        void start() {
            assert(
                m_manager.start() == 0 && 
                "Failed to start camera manager"
            );

            acquire_camera();
            configure_camera();
            create_allocator();

            assert(
                m_camera->start() == 0 &&
                "Failed to start camera"
            );
        }

        void latest_image() {

        }

    private:
        void acquire_camera() {
            auto cameras = m_manager.cameras();

            // there should be a camera connected to the pi
            assert(!cameras.empty() && "No cameras connected to device!");

            // only expecting one camera for this project
            m_camera = cameras[0];

            // this program should only be the one using the camera
            assert(m_camera->acquire() == 0 && "Camera is busy!");
        }

        void configure_camera() {
            m_config = m_camera->generateConfiguration({ 
                libcamera::StreamRole::Viewfinder 
            });

            assert(
                m_config != nullptr && 
                "Failed to generate camera configuration"
            );

            auto& stream_config = m_config->at(0);

            // apply config
            // stream_config.size.width = 640;
            // stream_config.size.height = 480;

            auto configuration_status = m_config->validate();

            if (configuration_status == libcamera::CameraConfiguration::Adjusted) {
                std::cout << "Camera configuration was adjusted!\n";
            }

            assert(
                configuration_status == libcamera::CameraConfiguration::Invalid &&
                "Invalid camera configuration given"
            );

            assert(
                m_camera->configure(m_config.get()) < 0 &&
                "Failed to configure camera!\n"
            );
        }

        void create_allocator() {
            m_allocator = std::make_unique<libcamera::FrameBufferAllocator>(m_camera);

            auto* view_finder_stream = m_config->at(0).stream(); 

            assert(
                m_allocator->allocate(view_finder_stream) >= 0 &&
                "Failed to allocate camera frame buffers"
            );
        }

        void create_requests() {
            auto* view_finder_stream = m_config->at(0).stream();
            const auto& frame_buffers = m_allocator->buffers(view_finder_stream);

            for (size_t i = 0; i < frame_buffers.size(); i++)  {
                auto request = m_camera->createRequest();
                const auto& frame_buffer = frame_buffers[i];

                assert(
                    request != nullptr &&
                    "Failed to create camera request"
                );

                assert(
                    request->addBuffer(view_finder_stream, frame_buffers.get()) == 0 &&
                    "Failed to add buffer to request"
                );

                m_requests.emplace_back(std::move(request));
            }
        }

    private:
        libcamera::CameraManager m_manager;

        std::shared_ptr<libcamera::Camera> m_camera;
        std::unique_ptr<CameraConfiguration> m_config;
        std::unique_ptr<libcamera::FrameBufferAllocator> m_allocator;

        std::vector<std::unique_ptr<libcamera::Request>> m_requests;
    };
}