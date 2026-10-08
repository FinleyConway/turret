#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include <libcamera/libcamera.h>
#include <opencv2/opencv.hpp>

// https://docs.libcamera.org/master/guides/application-developer.html
// https://github.com/erasta/libcamera-opencv/tree/main

namespace turret {
    class camera {
    public:
        camera() {
            assert(m_manager.start() == 0 && "Failed to start camera manager");

            acquire_camera();
            configure_camera();
            create_allocator();
            create_requests();

            m_camera->requestCompleted.connect(this, &camera::on_request_complete);
        }

        ~camera() {
            if (m_running) {
                m_camera->stop();
            }

            if (m_allocator != nullptr && m_config != nullptr) {
                m_allocator->free(m_config->at(0).stream());
                m_allocator.reset();
            }

            if (m_camera != nullptr) {
                m_camera->release();
                m_camera.reset();
            }

            m_manager.stop();
        }

    public:
        void start() {
            assert(!m_running);

            assert(m_camera->start() == 0 && "Failed to start camera");

            m_running = true;

            for (auto& request : m_requests) {
                assert(m_camera->queueRequest(request.get()) == 0 && "Failed to queue request");
            }
        }

        void stop() {
            if (!m_running) return;

            m_running = false;

            m_camera->stop();
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

            assert(m_config != nullptr && "Failed to generate camera configuration");

            // provide camera config such as camera size 
            auto& stream_config = m_config->at(0);

            // apply config
            //stream_config.size.width = 640;
            //stream_config.size.height = 480;

            // attempt to validate and apply given config
            auto configuration_status = m_config->validate();

            if (configuration_status == libcamera::CameraConfiguration::Adjusted) {
                std::cout << "Camera configuration was adjusted!\n";
            }

            assert(
                configuration_status != libcamera::CameraConfiguration::Invalid &&
                "Invalid camera configuration given"
            );

            assert(
                m_camera->configure(m_config.get()) == 0 &&
                "Failed to configure camera!\n"
            );
        }

        void create_allocator() {
            // create an allocator for the frame requests based on camera config
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
                    request->addBuffer(view_finder_stream, frame_buffer.get()) == 0 &&
                    "Failed to add buffer to request"
                );

                m_requests.emplace_back(std::move(request));
            }
        }

        void on_request_complete(libcamera::Request* req) {
            if (req->status() == libcamera::Request::RequestCancelled) return;

            for (const auto& [stream, buffer] : req->buffers()) {
                const auto cfg = stream->configuration();
                void* data = map_framebuffer(buffer);

                if (data == nullptr) continue;

                // need to test this?
                cv::Mat rgb(
                    cfg.size.height, 
                    cfg.size.width, 
                    CV_8UC3, 
                    data, 
                    cfg.stride
                );
                cv::cvtColor(rgb, m_image, cv::COLOR_RGB2BGR);

                static unsigned int frameNumber = 0;
                cv::imwrite(
                    "images/img" + std::to_string(frameNumber++) + ".png",
                    m_image
                );

                munmap(data, buffer->planes()[0].length);
            }

            if (!m_running) return;

            // recycle request to obtain another frame
            req->reuse(libcamera::Request::ReuseBuffers);
            assert(m_camera->queueRequest(req) == 0 && "Failed to requeue request");
        }

        // probably want to cache this process if im seeing the same fd
        void* map_framebuffer(libcamera::FrameBuffer* buffer) {
            auto& planes = buffer->planes();

            if (planes.empty()) return nullptr;

            const auto& plane = buffer->planes()[0];

            // look at the shared buffer used by the camera pipeline
            void* addr = mmap(
                nullptr,
                plane.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                plane.fd.get(),
                plane.offset
            );

            if (addr == MAP_FAILED) {
                std::cerr << "Failed to map frame buffer\n";
                
                return nullptr;
            }

            return addr;
        }

    private:
        libcamera::CameraManager m_manager;

        std::shared_ptr<libcamera::Camera> m_camera;
        std::unique_ptr<libcamera::CameraConfiguration> m_config;
        std::unique_ptr<libcamera::FrameBufferAllocator> m_allocator;
        std::vector<std::unique_ptr<libcamera::Request>> m_requests;

        cv::Mat m_image;
        bool m_running = false;
    };
}